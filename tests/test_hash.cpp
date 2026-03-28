#include <iostream>
#include <cassert>
#include <string>
#include "../src/hash.hpp"

void test_djb2_insertion_and_count() {
    HashTable table(100, HashFunc::DJB2);
    
    table.increment("computador");
    table.increment("computador");
    table.increment("algoritmo");

    assert(table.get("computador") == 2 && "Erro: 'computador' deveria ter contagem 2");
    assert(table.get("algoritmo") == 1 && "Erro: 'algoritmo' deveria ter contagem 1");
    assert(table.get("inexistente") == 0 && "Erro: Palavra não inserida deveria retornar 0");
    
    assert(table.size() == 2 && "Erro: Tabela deveria ter 2 palavras unicas");
}

void test_fnv1a_hashing() {
    HashTable table(100, HashFunc::FNV1A);
    table.increment("teste");
    
    assert(table.get("teste") == 1 && "Erro: FNV1A falhou ao buscar palavra");
}

int main() {
    std::cout << "Rodando testes da Tabela Hash...\n";
    
    test_djb2_insertion_and_count();
    test_fnv1a_hashing();
    
    std::cout << "✅ Todos os testes da Hash passaram!\n";
    return 0;
}