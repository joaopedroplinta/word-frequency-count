#pragma once
#include <cstdint>

// ============================================================
// Gerador de Números Pseudo-Aleatórios
//
// Método 1: LCG (Linear Congruential Generator)
//   - Clássico, rápido, período conhecido
//   - Parâmetros de Knuth (MMIX)
//
// Método 2: Xorshift64
//   - Período 2^64 - 1, excelente distribuição
//   - Muito mais rápido e melhor qualidade que LCG
// ============================================================

class RNG {
public:
    enum class Method { LCG, XORSHIFT };

    explicit RNG(uint64_t seed = 42, Method method = Method::LCG)
        : seed_(seed), state_(seed), method_(method) {}

    // Gera próximo inteiro sem sinal de 64 bits
    uint64_t next();

    // Gera inteiro no intervalo [0, n)
    uint64_t next_in(uint64_t n);

    // Gera double em [0.0, 1.0)
    double next_double();

    void reset() { state_ = seed_; }
    void set_seed(uint64_t s) { seed_ = s; state_ = s; }
    Method method() const { return method_; }

private:
    uint64_t seed_;
    uint64_t state_;
    Method method_;

    uint64_t lcg();
    uint64_t xorshift();
};