#include "hash.hpp"
#include <stdexcept>

HashTable::HashTable(size_t capacity, HashFunc func)
    : capacity_(capacity), func_(func), buckets_(capacity) {
    if (capacity == 0)
        throw std::invalid_argument("HashTable: capacidade deve ser >= 1");
}

HashTable::~HashTable() = default;

// djb2
size_t HashTable::djb2(const std::string& word) const {
    size_t h = 5381;
    for (unsigned char c : word)
        h = h * 33 + c;
    return h % capacity_;
}

// FNV-1a (64-bit) 
size_t HashTable::fnv1a(const std::string& word) const {
    uint64_t h = 14695981039346656037ULL; // FNV offset basis
    for (unsigned char c : word) {
        h ^= c;
        h *= 1099511628211ULL; // FNV prime
    }
    return static_cast<size_t>(h % capacity_);
}

size_t HashTable::hash(const std::string& word) const {
    return (func_ == HashFunc::DJB2) ? djb2(word) : fnv1a(word);
}

void HashTable::increment(const std::string& word) {
    ++stats_.inserts;
    size_t idx = hash(word);

    // Busca entrada existente no bucket
    for (HashEntry* e = buckets_[idx].get(); e != nullptr; e = e->next.get()) {
        if (e->key == word) {
            ++e->count;
            return;
        }
    }

    // Palavra nova: colisão só se o bucket já estava ocupado por outra palavra
    if (buckets_[idx] != nullptr) ++stats_.collisions;

    // Insere no início da lista do bucket
    auto entry = std::make_unique<HashEntry>(word, 1);
    entry->next = std::move(buckets_[idx]);
    buckets_[idx] = std::move(entry);
    ++stats_.unique_words;

    if (load_factor() > 0.75) rehash();
}

void HashTable::rehash() {
    size_t new_capacity = capacity_ * 2;
    std::vector<std::unique_ptr<HashEntry>> new_buckets(new_capacity);

    for (size_t i = 0; i < capacity_; ++i) {
        std::unique_ptr<HashEntry> e = std::move(buckets_[i]);
        while (e) {
            std::unique_ptr<HashEntry> next = std::move(e->next);

            // Recomputa índice com nova capacidade
            size_t h = 5381;
            if (func_ == HashFunc::FNV1A) {
                uint64_t fh = 14695981039346656037ULL;
                for (unsigned char c : e->key) { fh ^= c; fh *= 1099511628211ULL; }
                h = static_cast<size_t>(fh % new_capacity);
            } else {
                for (unsigned char c : e->key) h = h * 33 + c;
                h %= new_capacity;
            }

            e->next = std::move(new_buckets[h]);
            new_buckets[h] = std::move(e);
            e = std::move(next);
        }
    }

    capacity_ = new_capacity;
    buckets_ = std::move(new_buckets);
    ++stats_.rehashes;
}

int HashTable::get(const std::string& word) const {
    ++stats_.lookups;
    size_t idx = hash(word);
    for (HashEntry* e = buckets_[idx].get(); e != nullptr; e = e->next.get()) {
        if (e->key == word) return e->count;
    }
    return 0;
}