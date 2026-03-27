#include "hash.hpp"
#include <stdexcept>

HashTable::HashTable(size_t capacity, HashFunc func)
    : capacity_(capacity), func_(func), buckets_(capacity, nullptr) {}

HashTable::~HashTable() {
    for (size_t i = 0; i < capacity_; ++i) {
        HashEntry* e = buckets_[i];
        while (e) {
            HashEntry* next = e->next;
            delete e;
            e = next;
        }
    }
}

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
    if (buckets_[idx] != nullptr) ++stats_.collisions;

    for (HashEntry* e = buckets_[idx]; e != nullptr; e = e->next) {
        if (e->key == word) {
            ++e->count;
            return;
        }
    }

    // Insere no início da lista do bucket
    HashEntry* entry = new HashEntry(word, 1);
    entry->next = buckets_[idx];
    buckets_[idx] = entry;
    ++stats_.unique_words;
}

int HashTable::get(const std::string& word) const {
    ++stats_.lookups;
    size_t idx = hash(word);
    for (HashEntry* e = buckets_[idx]; e != nullptr; e = e->next) {
        if (e->key == word) return e->count;
    }
    return 0;
}