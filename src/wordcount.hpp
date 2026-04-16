#pragma once
#include "hash.hpp"
#include "heap.hpp"
#include "rng.hpp"
#include <string>
#include <vector>
#include <istream>
#include <chrono>

struct RunStats {
    size_t total_words    = 0;
    size_t unique_words   = 0;
    size_t hash_inserts   = 0;
    size_t hash_lookups   = 0;
    size_t hash_collisions= 0;
    size_t hash_rehashes  = 0;
    size_t hash_capacity  = 0;
    size_t heap_ops       = 0;
    double load_factor    = 0.0;
    long long time_ms     = 0;
    long   memory_kb    = 0;
};

class WordCounter {
public:
    WordCounter(size_t hash_capacity, HashFunc hash_func);

    // Lê e conta palavras de um stream (arquivo ou stdin)
    void count_from_stream(std::istream& in);

    // Gera palavras aleatórias e conta
    void count_from_random(size_t num_words, RNG& rng);

    // Retorna as k palavras mais frequentes (a partir do offset)
    std::vector<HeapNode> top_k(size_t k, size_t offset = 0);

    // Retorna as k palavras menos frequentes (a partir do offset)
    std::vector<HeapNode> bottom_k(size_t k, size_t offset = 0);

    const RunStats& stats() const { return stats_; }
    void print_stats() const;

private:
    HashTable table_;
    RunStats  stats_;

    std::string normalize(const std::string& word) const;
};