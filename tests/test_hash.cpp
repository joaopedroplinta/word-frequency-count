#include "../src/hash.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>

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

// T1: Inserção e consulta básica
void test_basic_insert_get() {
    std::cout << "\n[T1] Inserção e consulta básica\n";

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(64, func);

        ht.increment("hello");
        ASSERT(ht.get("hello") == 1, std::string(name) + ": primeira inserção conta 1");

        ht.increment("hello");
        ht.increment("hello");
        ASSERT(ht.get("hello") == 3, std::string(name) + ": incrementar 3x retorna 3");

        ASSERT(ht.get("world") == 0, std::string(name) + ": palavra ausente retorna 0");

        ht.increment("world");
        ASSERT(ht.get("world") == 1, std::string(name) + ": segunda palavra inserida corretamente");
    }
}

// T2: Tamanho e palavras únicas
void test_size() {
    std::cout << "\n[T2] Tamanho (palavras únicas)\n";

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(256, func);

        ASSERT(ht.size() == 0, std::string(name) + ": tabela vazia tem size 0");

        std::vector<std::string> words = {"a", "b", "c", "d", "e"};
        for (auto& w : words) {
            ht.increment(w);
            ht.increment(w); // incrementar duas vezes não aumenta unique_words
        }

        ASSERT(ht.size() == 5, std::string(name) + ": 5 palavras únicas mesmo com múltiplos incrementos");
    }
}

// T3: Corretude com muitas palavras — comparação com std::unordered_map
void test_correctness_vs_reference() {
    std::cout << "\n[T3] Corretude vs std::unordered_map (referência)\n";

    std::vector<std::string> words = {
        "the","be","to","of","and","a","in","that","have","it",
        "for","not","on","with","he","as","you","do","at","this",
        "the","be","a","in","the","and","of","the","to","the"
    };

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(64, func);
        std::unordered_map<std::string, int> ref;

        for (auto& w : words) {
            ht.increment(w);
            ++ref[w];
        }

        bool match = true;
        for (auto& [w, c] : ref) {
            if (ht.get(w) != c) { match = false; break; }
        }
        ASSERT(match, std::string(name) + ": contagens idênticas ao unordered_map de referência");
        ASSERT(ht.size() == ref.size(), std::string(name) + ": número de palavras únicas idêntico");
    }
}

// T4: for_each itera sobre todos os elementos
void test_for_each() {
    std::cout << "\n[T4] for_each cobre todos os elementos\n";

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(64, func);

        std::vector<std::string> inserted = {"alpha","beta","gamma","delta","epsilon"};
        for (auto& w : inserted) ht.increment(w);

        std::vector<std::string> seen;
        int total_count = 0;
        ht.for_each([&](const std::string& w, int c) {
            seen.push_back(w);
            total_count += c;
        });

        ASSERT(seen.size() == inserted.size(), std::string(name) + ": for_each visita todas as palavras únicas");
        ASSERT(total_count == (int)inserted.size(), std::string(name) + ": soma das contagens == total inserido");
    }
}

// T5: Capacidade e fator de carga
void test_load_factor() {
    std::cout << "\n[T5] Fator de carga\n";

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(100, func);

        for (int i = 0; i < 50; ++i)
            ht.increment("palavra" + std::to_string(i));

        double lf = ht.load_factor();
        ASSERT(lf > 0.0 && lf <= 1.0, std::string(name) + ": fator de carga em (0, 1]");

        double expected = 50.0 / 100.0;
        bool close = std::abs(lf - expected) < 0.01;
        ASSERT(close, std::string(name) + ": fator de carga ≈ 0.50 com 50/100 elementos");
    }
}

// T6: Colisões são contabilizadas nas stats
void test_collision_counting() {
    std::cout << "\n[T6] Estatísticas de colisão\n";

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";

        // Inserir 200 palavras distintas com capacidade inicial 64.
        // Nas primeiras ~48 inserções (antes do primeiro rehash) ocorrem colisões
        // garantidas por volume (E[colisões] ≈ 17 nessa fase).
        const int N = 200;
        HashTable ht(64, func);
        for (int i = 0; i < N; ++i)
            ht.increment("word_" + std::to_string(i));

        ASSERT(ht.stats().collisions > 0,
               std::string(name) + ": 200 inserções geram colisões (verificado por volume)");
        ASSERT(ht.stats().inserts == N,
               std::string(name) + ": " + std::to_string(N) + " inserts registrados nas stats");
    }
}

// T7: Palavras com caracteres variados
void test_edge_cases() {
    std::cout << "\n[T7] Casos extremos\n";

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(64, func);

        // Palavra de um caractere
        ht.increment("a");
        ASSERT(ht.get("a") == 1, std::string(name) + ": palavra de 1 char");

        // Palavra longa
        std::string long_word(200, 'x');
        ht.increment(long_word);
        ASSERT(ht.get(long_word) == 1, std::string(name) + ": palavra de 200 chars");

        // Palavras que se diferenciam apenas no final
        ht.increment("prefixo_a");
        ht.increment("prefixo_b");
        ASSERT(ht.get("prefixo_a") == 1 && ht.get("prefixo_b") == 1,
               std::string(name) + ": palavras similares armazenadas separadamente");
    }
}

// T8: Distribuição dos buckets — nenhum bucket com mais de 5x a média
void test_bucket_distribution() {
    std::cout << "\n[T8] Distribuição de buckets (qualidade do espalhamento)\n";

    const int N = 1000;
    const size_t CAP = 256;

    for (auto func : {HashFunc::DJB2, HashFunc::FNV1A}) {
        const char* name = (func == HashFunc::DJB2) ? "djb2" : "fnv1a";
        HashTable ht(CAP, func);

        for (int i = 0; i < N; ++i)
            ht.increment("palavra_" + std::to_string(i));

        // Com fator de carga ~3.9 (1000/256), colisões altas são esperadas.
        // Validamos apenas sanidade: taxa < 99%.
        double col_rate = static_cast<double>(ht.stats().collisions) / ht.stats().inserts;
        double lf = ht.load_factor();
        std::cout << "    " << name << " col=" << std::fixed << std::setprecision(1)
                  << col_rate*100 << "% fator_carga=" << lf << "\n";
        ASSERT(col_rate < 0.99, std::string(name) + ": taxa de colisão < 99% (sanidade)");
    }
}

// Main
int main() {
    std::cout << "================================================\n";
    std::cout << " Testes Unitários — HashTable\n";
    std::cout << "================================================\n";

    test_basic_insert_get();
    test_size();
    test_correctness_vs_reference();
    test_for_each();
    test_load_factor();
    test_collision_counting();
    test_edge_cases();
    test_bucket_distribution();

    std::cout << "\n================================================\n";
    std::cout << " Resultado: " << passed << " OK, " << failed << " FALHOU\n";
    std::cout << "================================================\n";

    return (failed == 0) ? 0 : 1;
}