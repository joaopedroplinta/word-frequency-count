#include "rng.hpp"
#include <stdexcept>

// LCG (Linear Congruential Generator): state = a * state + c  (mod 2^64)
// Constantes de Knuth (MMIX): a = 6364136223846793005, c = 1442695040888963407
// Período máximo de 2^64 com essas constantes.
uint64_t RNG::lcg() {
    state_ = 6364136223846793005ULL * state_ + 1442695040888963407ULL;
    return state_;
}

// Xorshift64: aplica três deslocamentos XOR para embaralhar os bits.
// Shifts (13, 7, 17) formam um conjunto de período máximo 2^64 - 1.
// Estado 0 é absorvente (geraria zeros infinitos), então é forçado para 1.
uint64_t RNG::xorshift() {
    if (state_ == 0) state_ = 1;
    state_ ^= state_ << 13;
    state_ ^= state_ >> 7;
    state_ ^= state_ << 17;
    return state_;
}

uint64_t RNG::next() {
    return (method_ == Method::LCG) ? lcg() : xorshift();
}

uint64_t RNG::next_in(uint64_t n) {
    if (n == 0)
        throw std::invalid_argument("RNG::next_in: n deve ser >= 1");
    return next() % n;
}

double RNG::next_double() {
    return static_cast<double>(next()) / static_cast<double>(UINT64_MAX);
}