#include "../src/rng.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>

// Utilitários de teste
static int passed = 0;
static int failed = 0;

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "  [FALHOU] " << msg << "\n"; \
            ++failed; \
        } else { \
            std::cout << "  [OK]     " << msg << "\n"; \
            ++passed; \
        } \
    } while(0)

// T1: Reprodutibilidade — mesma seed gera mesma sequência
void test_reproducibility() {
    std::cout << "\n[T1] Reprodutibilidade\n";

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG a(42, method), b(42, method);

        bool same = true;
        for (int i = 0; i < 1000; ++i)
            if (a.next() != b.next()) { same = false; break; }

        ASSERT(same, std::string(name) + ": mesma seed -> mesma sequência (1000 valores)");

        // Seeds diferentes devem gerar sequências diferentes
        RNG c(42, method), d(99, method);
        bool diff = false;
        for (int i = 0; i < 10; ++i)
            if (c.next() != d.next()) { diff = true; break; }

        ASSERT(diff, std::string(name) + ": seeds diferentes -> sequências diferentes");
    }
}

// T2: Reset restaura a sequência ao início
void test_reset() {
    std::cout << "\n[T2] Reset\n";

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG rng(42, method);

        std::vector<uint64_t> first(100), second(100);
        for (auto& v : first)  v = rng.next();
        rng.reset();
        for (auto& v : second) v = rng.next();

        ASSERT(first == second, std::string(name) + ": reset restaura sequência original");
    }
}

// T3: next_in respeita o intervalo [0, n)
void test_range() {
    std::cout << "\n[T3] next_in respeita [0, n)\n";

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG rng(123, method);

        bool ok = true;
        for (int i = 0; i < 100000; ++i) {
            uint64_t v = rng.next_in(100);
            if (v >= 100) { ok = false; break; }
        }
        ASSERT(ok, std::string(name) + ": next_in(100) sempre em [0,100)");

        // Testa com n=1 (deve sempre retornar 0)
        bool always_zero = true;
        for (int i = 0; i < 100; ++i)
            if (rng.next_in(1) != 0) { always_zero = false; break; }
        ASSERT(always_zero, std::string(name) + ": next_in(1) sempre retorna 0");
    }
}

// T4: next_double em [0.0, 1.0)
void test_double_range() {
    std::cout << "\n[T4] next_double em [0.0, 1.0)\n";

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG rng(42, method);

        bool ok = true;
        for (int i = 0; i < 100000; ++i) {
            double v = rng.next_double();
            if (v < 0.0 || v >= 1.0) { ok = false; break; }
        }
        ASSERT(ok, std::string(name) + ": next_double sempre em [0.0, 1.0)");
    }
}

// T5: Distribuição uniforme — teste qui-quadrado simplificado
//     Divide [0, n) em buckets e verifica que nenhum bucket
//     desvia mais de 20% da média esperada
void test_distribution() {
    std::cout << "\n[T5] Distribuição uniforme (qui-quadrado simplificado)\n";

    const int    BUCKETS  = 10;
    const int    SAMPLES  = 100000;
    const double EXPECTED = static_cast<double>(SAMPLES) / BUCKETS;
    const double TOLERANCE = 0.07; // 7% de desvio máximo

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG rng(42, method);

        std::vector<int> buckets(BUCKETS, 0);
        for (int i = 0; i < SAMPLES; ++i)
            ++buckets[rng.next_in(BUCKETS)];

        double chi2 = 0.0;
        bool ok = true;
        for (int b = 0; b < BUCKETS; ++b) {
            double dev = std::abs(buckets[b] - EXPECTED) / EXPECTED;
            if (dev > TOLERANCE) ok = false;
            chi2 += (buckets[b] - EXPECTED) * (buckets[b] - EXPECTED) / EXPECTED;
        }

        std::cout << "    " << name << " chi2=" << std::fixed << std::setprecision(2) << chi2
                  << " (esperado < " << BUCKETS * 25 << " para distribuição uniforme)\n";
        ASSERT(ok, std::string(name) + ": desvio por bucket <= 7% da média esperada");
    }
}

// T6: Não gera zeros consecutivos (Xorshift tem estado zero proibido)
void test_no_zero_state() {
    std::cout << "\n[T6] Robustez com seed 0\n";

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG rng(0, method);

        bool all_zero = true;
        for (int i = 0; i < 100; ++i)
            if (rng.next() != 0) { all_zero = false; break; }

        ASSERT(!all_zero, std::string(name) + ": seed 0 não produz sequência constante de zeros");
    }
}

// T7: Comparação de período aparente — Xorshift deve ter
//     período muito maior que LCG para sequências curtas
void test_non_repeating() {
    std::cout << "\n[T7] Ausência de repetições em janela de 10000 valores\n";

    for (auto method : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
        const char* name = (method == RNG::Method::LCG) ? "LCG" : "Xorshift";
        RNG rng(42, method);

        const int N = 10000;
        std::vector<uint64_t> vals(N);
        for (auto& v : vals) v = rng.next();

        std::sort(vals.begin(), vals.end());
        bool no_repeat = (std::unique(vals.begin(), vals.end()) == vals.end());
        ASSERT(no_repeat, std::string(name) + ": sem repetições em 10000 valores consecutivos");
    }
}

// Main
int main() {
    std::cout << "================================================\n";
    std::cout << " Testes Unitários — RNG\n";
    std::cout << "================================================\n";

    test_reproducibility();
    test_reset();
    test_range();
    test_double_range();
    test_distribution();
    test_no_zero_state();
    test_non_repeating();

    std::cout << "\n================================================\n";
    std::cout << " Resultado: " << passed << " OK, " << failed << " FALHOU\n";
    std::cout << "================================================\n";

    return (failed == 0) ? 0 : 1;
}