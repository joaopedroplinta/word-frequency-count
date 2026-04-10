#include "wordcount.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sys/resource.h>

static long rss_kb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // KB no Linux
}

// Vocabulário para geração de texto aleatório
static const std::vector<std::string> VOCAB = {
    "the","be","to","of","and","a","in","that","have","it",
    "for","not","on","with","he","as","you","do","at","this",
    "but","his","by","from","they","we","say","her","she","or",
    "an","will","my","one","all","would","there","their","what",
    "so","up","out","if","about","who","get","which","go","me",
    "when","make","can","like","time","no","just","him","know",
    "take","people","into","year","your","good","some","could",
    "them","see","other","than","then","now","look","only","come",
    "its","over","think","also","back","after","use","two","how",
    "our","work","first","well","way","even","new","want","because",
    "any","these","give","day","most","us","data","hash","heap","word"
};

WordCounter::WordCounter(size_t hash_capacity, HashFunc hash_func)
    : table_(hash_capacity, hash_func) {}

std::string WordCounter::normalize(const std::string& word) const {
    std::string result;
    result.reserve(word.size());
    for (char c : word) {
        if (std::isalpha(static_cast<unsigned char>(c)))
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

void WordCounter::count_from_stream(std::istream& in) {
    auto t0 = std::chrono::high_resolution_clock::now();

    std::string token;
    while (in >> token) {
        std::string word = normalize(token);
        if (!word.empty()) {
            table_.increment(word);
            ++stats_.total_words;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    stats_.time_ms        = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    stats_.unique_words   = table_.size();
    stats_.hash_inserts   = table_.stats().inserts;
    stats_.hash_lookups   = table_.stats().lookups;
    stats_.hash_collisions= table_.stats().collisions;
    stats_.hash_rehashes  = table_.stats().rehashes;
    stats_.hash_capacity  = table_.capacity();
    stats_.load_factor    = table_.load_factor();
    stats_.memory_kb      = rss_kb();
}

void WordCounter::count_from_random(size_t num_words, RNG& rng) {
    auto t0 = std::chrono::high_resolution_clock::now();

    size_t vocab_size = VOCAB.size();
    for (size_t i = 0; i < num_words; ++i) {
        const std::string& word = VOCAB[rng.next_in(vocab_size)];
        table_.increment(word);
        ++stats_.total_words;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    stats_.time_ms        = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    stats_.unique_words   = table_.size();
    stats_.hash_inserts   = table_.stats().inserts;
    stats_.hash_lookups   = table_.stats().lookups;
    stats_.hash_collisions= table_.stats().collisions;
    stats_.hash_rehashes  = table_.stats().rehashes;
    stats_.hash_capacity  = table_.capacity();
    stats_.load_factor    = table_.load_factor();
    stats_.memory_kb      = rss_kb();
}

std::vector<HeapNode> WordCounter::top_k(size_t k) {
    if (k == 0) return {};

    auto t0 = std::chrono::high_resolution_clock::now();

    // Coleta todos os pares (palavra, contagem) da hash: O(n)
    std::vector<HeapNode> all;
    all.reserve(table_.size());
    table_.for_each([&](const std::string& w, int c) {
        all.emplace_back(w, c);
    });

    // Constrói heap via heapify: O(n)
    MaxHeap heap(std::move(all));

    // Extrai os k maiores diretamente do heap local (sem cópia): O(k log n)
    size_t limit = std::min(k, heap.size());
    std::vector<HeapNode> result;
    result.reserve(limit);
    for (size_t i = 0; i < limit; ++i) {
        result.push_back(heap.top());
        heap.pop();
    }
    stats_.heap_ops = heap.ops();

    auto t1 = std::chrono::high_resolution_clock::now();
    stats_.time_ms += std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    return result;
}

void WordCounter::print_stats() const {
    std::cout << "\n========== Estatísticas ==========\n";
    std::cout << std::left << std::setw(30) << "Total de palavras:"    << stats_.total_words     << "\n";
    std::cout << std::left << std::setw(30) << "Palavras únicas:"      << stats_.unique_words    << "\n";
    std::cout << std::left << std::setw(30) << "Inserções na hash:"    << stats_.hash_inserts    << "\n";
    std::cout << std::left << std::setw(30) << "Colisões na hash:"     << stats_.hash_collisions << "\n";
    std::cout << std::left << std::setw(30) << "Rehashes:"             << stats_.hash_rehashes   << "\n";
    std::cout << std::left << std::setw(30) << "Capacidade final:"     << stats_.hash_capacity   << "\n";
    std::cout << std::left << std::setw(30) << "Fator de carga:"
              << std::fixed << std::setprecision(4) << stats_.load_factor << "\n";
    std::cout << std::left << std::setw(30) << "Operações no heap:"    << stats_.heap_ops        << "\n";
    std::cout << std::left << std::setw(30) << "Tempo total (ms):"     << stats_.time_ms         << "\n";
    std::cout << std::left << std::setw(30) << "Memória RSS (KB):"     << stats_.memory_kb       << "\n";
    std::cout << "==================================\n";
}