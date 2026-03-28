#include <iostream>
#include <cassert>
#include "../src/rng.hpp"

void test_deterministic_lcg() {
    RNG rng1(42, RNG::Method::LCG);
    RNG rng2(42, RNG::Method::LCG);
    
    // Testa as primeiras 100 gerações para garantir a estabilidade
    for (int i = 0; i < 100; ++i) {
        assert(rng1.next() == rng2.next() && "Erro: LCG falhou na reprodutibilidade!");
    }
}

void test_deterministic_xorshift() {
    RNG rng1(12345, RNG::Method::XORSHIFT);
    RNG rng2(12345, RNG::Method::XORSHIFT);
    
    for (int i = 0; i < 100; ++i) {
        assert(rng1.next() == rng2.next() && "Erro: Xorshift falhou na reprodutibilidade!");
    }
}

int main() {
    std::cout << "Rodando testes do RNG...\n";
    
    test_deterministic_lcg();
    test_deterministic_xorshift();
    
    std::cout << "✅ Todos os testes de RNG passaram!\n";
    return 0;
}