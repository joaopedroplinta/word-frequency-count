#include "../src/heap.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>
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

// T1: Push e top básico
void test_push_top() {
    std::cout << "\n[T1] Push e top\n";

    MaxHeap h;
    ASSERT(h.empty(), "heap começa vazio");

    h.push("b", 5);
    ASSERT(h.top().count == 5, "top após 1 inserção");
    ASSERT(h.size() == 1,       "size == 1");

    h.push("a", 10);
    ASSERT(h.top().count == 10, "elemento maior vira raiz");
    ASSERT(h.top().word == "a", "palavra correta na raiz");

    h.push("c", 3);
    ASSERT(h.top().count == 10, "raiz permanece o maior após 3ª inserção");
    ASSERT(h.size() == 3,       "size == 3");
}

// T2: Pop remove em ordem decrescente
void test_pop_order() {
    std::cout << "\n[T2] Pop em ordem decrescente\n";

    MaxHeap h;
    std::vector<int> counts = {7, 2, 9, 4, 6, 1, 8, 3, 5, 10};
    for (int i = 0; i < (int)counts.size(); ++i)
        h.push("w" + std::to_string(i), counts[i]);

    std::vector<int> extracted;
    while (!h.empty()) {
        extracted.push_back(h.top().count);
        h.pop();
    }

    bool sorted = std::is_sorted(extracted.rbegin(), extracted.rend());
    ASSERT(sorted, "pop extrai elementos em ordem decrescente");

    std::sort(counts.begin(), counts.end(), std::greater<int>());
    ASSERT(extracted == counts, "sequência extraída corresponde à ordenação correta");
}

// T3: top_k retorna exatamente k elementos em ordem
void test_top_k() {
    std::cout << "\n[T3] top_k\n";

    MaxHeap h;
    for (int i = 1; i <= 20; ++i)
        h.push("palavra_" + std::to_string(i), i);

    // top_5 deve retornar 20,19,18,17,16
    MaxHeap h2;
    for (int i = 1; i <= 20; ++i)
        h2.push("palavra_" + std::to_string(i), i);

    auto top5 = h2.top_k(5);
    ASSERT(top5.size() == 5, "top_k(5) retorna exatamente 5 elementos");
    ASSERT(top5[0].count == 20, "1º elemento é o maior (20)");
    ASSERT(top5[1].count == 19, "2º elemento é 19");
    ASSERT(top5[4].count == 16, "5º elemento é 16");

    // top_k maior que o tamanho retorna tudo
    MaxHeap h3;
    for (int i = 0; i < 3; ++i)
        h3.push("x" + std::to_string(i), i);

    auto all = h3.top_k(100);
    ASSERT(all.size() == 3, "top_k(100) em heap de 3 retorna 3 elementos");
}

// T4: Heapify (construção a partir de vetor) — O(n)
void test_heapify() {
    std::cout << "\n[T4] Heapify (construção em O(n))\n";

    std::vector<HeapNode> nodes;
    for (int i = 0; i < 100; ++i)
        nodes.emplace_back("w" + std::to_string(i), i);

    // Embaralha para garantir que heapify de fato ordena
    std::reverse(nodes.begin(), nodes.end());

    MaxHeap h(nodes);
    ASSERT(h.top().count == 99, "heapify: raiz é o maior elemento (99)");
    ASSERT(h.size() == 100,     "heapify: size == 100");

    // Extrai tudo e verifica ordem
    std::vector<int> extracted;
    while (!h.empty()) {
        extracted.push_back(h.top().count);
        h.pop();
    }
    ASSERT(std::is_sorted(extracted.rbegin(), extracted.rend()),
           "heapify: extração em ordem decrescente correta");
}

// T5: Heap vazio lança exceção
void test_empty_throws() {
    std::cout << "\n[T5] Operações em heap vazio\n";

    MaxHeap h;

    bool top_threw = false;
    try { h.top(); } catch (const std::underflow_error&) { top_threw = true; }
    ASSERT(top_threw, "top() em heap vazio lança underflow_error");

    bool pop_threw = false;
    try { h.pop(); } catch (const std::underflow_error&) { pop_threw = true; }
    ASSERT(pop_threw, "pop() em heap vazio lança underflow_error");
}

// T6: Elementos com contagem igual são tratados corretamente
void test_equal_counts() {
    std::cout << "\n[T6] Elementos com contagem igual\n";

    MaxHeap h;
    h.push("a", 5);
    h.push("b", 5);
    h.push("c", 5);
    h.push("d", 10);

    ASSERT(h.top().count == 10, "maior único ainda é raiz com empates");

    h.pop();
    ASSERT(h.top().count == 5, "após remover o maior, raiz é um dos empatados (5)");
    ASSERT(h.size() == 3,      "size correto após pop");

    auto topk = h.top_k(3);
    for (auto& n : topk)
        ASSERT(n.count == 5, "todos os restantes têm count 5");
}

// T7: Heap com 1 elemento
void test_single_element() {
    std::cout << "\n[T7] Heap com 1 elemento\n";

    MaxHeap h;
    h.push("unico", 42);

    ASSERT(h.size() == 1,           "size == 1");
    ASSERT(h.top().count == 42,     "top correto");
    ASSERT(h.top().word == "unico", "palavra correta");

    h.pop();
    ASSERT(h.empty(), "heap vazio após remover único elemento");
}

// T8: Contagem de operações (ops) cresce com o heap
void test_ops_counting() {
    std::cout << "\n[T8] Contagem de operações\n";

    MaxHeap h1, h2;
    for (int i = 0; i < 10;  ++i) h1.push("w" + std::to_string(i), i);
    for (int i = 0; i < 100; ++i) h2.push("w" + std::to_string(i), i);

    // Heap maior deve ter feito mais operações (sift-up mais comparações)
    ASSERT(h2.ops() > h1.ops(), "heap maior acumula mais operações de sift");
    ASSERT(h1.ops() > 0,        "operações contadas (> 0) após inserções");
}

// Main
int main() {
    std::cout << "================================================\n";
    std::cout << " Testes Unitários — MaxHeap\n";
    std::cout << "================================================\n";

    test_push_top();
    test_pop_order();
    test_top_k();
    test_heapify();
    test_empty_throws();
    test_equal_counts();
    test_single_element();
    test_ops_counting();

    std::cout << "\n================================================\n";
    std::cout << " Resultado: " << passed << " OK, " << failed << " FALHOU\n";
    std::cout << "================================================\n";

    return (failed == 0) ? 0 : 1;
}