#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstddef>

// Tabela Hash para contagem de frequência de palavras
// Colisões resolvidas por encadeamento (linked list por bucket)
//
// Função 1: djb2
//   - Dan Bernstein, amplamente usada, simples e eficaz
//   - hash = hash * 33 + c  (iniciando em 5381)
//
// Função 2: FNV-1a (Fowler–Noll–Vo)
//   - Excelente avalanche, baixa taxa de colisão
//   - hash = (hash XOR c) * prime

enum class HashFunc { DJB2, FNV1A };

struct HashEntry {
    std::string key;
    int count = 0;
    std::unique_ptr<HashEntry> next;

    HashEntry(const std::string& k, int c) : key(k), count(c) {}
};

struct HashStats {
    size_t inserts      = 0;
    size_t lookups      = 0;
    size_t collisions   = 0;
    size_t unique_words = 0;
    size_t rehashes     = 0;
};

class HashTable {
public:
    explicit HashTable(size_t capacity = 16384, HashFunc func = HashFunc::DJB2);
    ~HashTable();

    void increment(const std::string& word);

    int get(const std::string& word) const;

    // Itera sobre todos os pares (palavra, contagem)
    template<typename Fn>
    void for_each(Fn callback) const {
        for (size_t i = 0; i < capacity_; ++i) {
            for (HashEntry* e = buckets_[i].get(); e != nullptr; e = e->next.get())
                callback(e->key, e->count);
        }
    }

    size_t capacity()    const { return capacity_; }
    size_t size()        const { return stats_.unique_words; }
    const HashStats& stats() const { return stats_; }
    double load_factor() const { return static_cast<double>(stats_.unique_words) / capacity_; }

private:
    size_t capacity_;
    HashFunc func_;
    std::vector<std::unique_ptr<HashEntry>> buckets_;
    mutable HashStats stats_;

    void   rehash();
    size_t hash(const std::string& word) const;
    size_t djb2(const std::string& word) const;
    size_t fnv1a(const std::string& word) const;
};