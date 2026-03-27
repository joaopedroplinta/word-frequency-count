#include "rng.hpp"

// ---- LCG: x_{n+1} = (a * x_n + c) mod m ----
// Parâmetros de Knuth (MMIX): a=6364136223846793005, c=1442695040888963407, m=2^64
uint64_t RNG::lcg() {
    state_ = 6364136223846793005ULL * state_ + 1442695040888963407ULL;
    return state_;
}

// ---- Xorshift64 ----
// Período: 2^64 - 1, sem estado zero
uint64_t RNG::xorshift() {
    if (state_ == 0) state_ = 1; // estado zero é absorvente
    state_ ^= state_ << 13;
    state_ ^= state_ >> 7;
    state_ ^= state_ << 17;
    return state_;
}

uint64_t RNG::next() {
    return (method_ == Method::LCG) ? lcg() : xorshift();
}

uint64_t RNG::next_in(uint64_t n) {
    return next() % n;
}

double RNG::next_double() {
    return static_cast<double>(next()) / static_cast<double>(UINT64_MAX);
}