#include <iostream>
#include <cassert>
#include <vector>
#include "../src/heap.hpp"

void test_push_pop_max_heap() {
    MaxHeap heap;
    
    heap.push("baixa", 5);
    heap.push("alta", 20);
    heap.push("media", 10);
    
    // O maior (20) deve estar no topo
    assert(heap.top().word == "alta" && heap.top().count == 20 && "Erro: Topo incorreto");
    
    heap.pop();
    // O proximo maior (10) deve assumir o topo
    assert(heap.top().word == "media" && heap.top().count == 10 && "Erro: Sift-down falhou");
}

void test_vector_constructor_and_top_k() {
    std::vector<HeapNode> data = {
        HeapNode("a", 1),
        HeapNode("b", 50),
        HeapNode("c", 30),
        HeapNode("d", 15)
    };
    
    // Constroi heap em O(n)
    MaxHeap heap(data);
    
    // Extrai o top 2
    auto k_nodes = heap.top_k(2);
    
    assert(k_nodes.size() == 2 && "Erro: Tamanho do top_k incorreto");
    assert(k_nodes[0].word == "b" && k_nodes[0].count == 50 && "Erro: 1º do top_k incorreto");
    assert(k_nodes[1].word == "c" && k_nodes[1].count == 30 && "Erro: 2º do top_k incorreto");
}

int main() {
    std::cout << "Rodando testes do Heap...\n";
    
    test_push_pop_max_heap();
    test_vector_constructor_and_top_k();
    
    std::cout << "✅ Todos os testes do Heap passaram!\n";
    return 0;
}