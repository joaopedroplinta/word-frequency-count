#include "../src/hash.hpp"
#include "../src/heap.hpp"
#include "../src/rng.hpp"
#include "../src/wordcount.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cmath>

// Escapa string para JSON
static std::string jstr(const std::string& s) { return "\"" + s + "\""; }
static std::string jf(double v, int p = 4) {
    std::ostringstream ss; ss << std::fixed << std::setprecision(p) << v; return ss.str();
}

struct BenchResult {
    std::string label;
    std::string hash_func;
    std::string rng_method;
    size_t      input_size;
    size_t      initial_capacity;
    size_t      unique_words;
    size_t      collisions;
    size_t      rehashes;
    size_t      hash_capacity;
    double      load_factor;
    size_t      heap_ops;
    long long   time_ms;
    long        memory_kb;
    bool        is_file;
    std::string filename;
};

// Roda um benchmark e retorna resultado
BenchResult run_bench(size_t n, uint64_t seed, HashFunc hf, RNG::Method rm,
                      bool from_file, const std::string& filepath,
                      size_t initial_capacity = 16384) {
    BenchResult r;
    r.hash_func  = (hf == HashFunc::DJB2) ? "djb2" : "fnv1a";
    r.rng_method = (rm == RNG::Method::LCG) ? "LCG" : "Xorshift64";
    r.is_file           = from_file;
    r.filename          = filepath;
    r.input_size        = n;
    r.initial_capacity  = initial_capacity;

    WordCounter wc(initial_capacity, hf);

    if (from_file) {
        std::ifstream f(filepath);
        if (!f) { r.label = "ERRO"; return r; }
        wc.count_from_stream(f);
    } else {
        RNG rng(seed, rm);
        wc.count_from_random(n, rng);
    }

    auto top = wc.top_k(10);

    auto& st = wc.stats();
    r.unique_words  = st.unique_words;
    r.collisions    = st.hash_collisions;
    r.rehashes      = st.hash_rehashes;
    r.hash_capacity = st.hash_capacity;
    r.load_factor   = st.load_factor;
    r.heap_ops      = st.heap_ops;
    r.time_ms       = st.time_ms;
    r.memory_kb     = st.memory_kb;
    r.label        = r.hash_func + "+" + r.rng_method + "_n" + std::to_string(n);
    return r;
}

int main(int argc, char* argv[]) {
    std::string out_file = (argc > 1) ? argv[1] : "benchmark_results.json";

    std::vector<BenchResult> results;

    // 1. Escala de tamanho — djb2 vs fnv1a, LCG seed=42
    std::vector<size_t> sizes = {1000, 5000, 10000, 50000, 100000, 500000};
    for (size_t n : sizes) {
        for (auto hf : {HashFunc::DJB2, HashFunc::FNV1A}) {
            auto r = run_bench(n, 42, hf, RNG::Method::LCG, false, "");
            results.push_back(r);
        }
    }

    // 2. LCG vs Xorshift — djb2, n=100000, várias seeds
    std::vector<uint64_t> seeds = {42, 123, 9999, 314159, 777, 1000000};
    for (uint64_t seed : seeds) {
        for (auto rm : {RNG::Method::LCG, RNG::Method::XORSHIFT}) {
            auto r = run_bench(100000, seed, HashFunc::DJB2, rm, false, "");
            r.label = "rng_" + r.rng_method + "_seed" + std::to_string(seed);
            results.push_back(r);
        }
    }

    // 3. Arquivos reais
    std::vector<std::string> files = {
        "inputs/texto_real_pequeno.txt",
        "inputs/texto_real_medio.txt",
        "inputs/texto_real_grande.txt"
    };
    for (auto& f : files) {
        for (auto hf : {HashFunc::DJB2, HashFunc::FNV1A}) {
            auto r = run_bench(0, 0, hf, RNG::Method::LCG, true, f);
            results.push_back(r);
        }
    }

    // 4. Impacto da capacidade inicial no rehash — djb2, n=10000, seed=42
    std::vector<size_t> caps = {8, 16, 32, 64, 128, 256, 512, 1024, 16384};
    for (size_t cap : caps) {
        auto r = run_bench(10000, 42, HashFunc::DJB2, RNG::Method::LCG, false, "", cap);
        r.label = "rehash_cap" + std::to_string(cap);
        results.push_back(r);
    }

    // Escreve JSON
    std::ofstream out(out_file);
    out << "[\n";
    for (size_t i = 0; i < results.size(); ++i) {
        auto& r = results[i];
        out << "  {\n";
        out << "    " << jstr("label")             << ": " << jstr(r.label)           << ",\n";
        out << "    " << jstr("hash_func")         << ": " << jstr(r.hash_func)       << ",\n";
        out << "    " << jstr("rng_method")        << ": " << jstr(r.rng_method)      << ",\n";
        out << "    " << jstr("input_size")        << ": " << r.input_size            << ",\n";
        out << "    " << jstr("initial_capacity")  << ": " << r.initial_capacity      << ",\n";
        out << "    " << jstr("unique_words") << ": " << r.unique_words       << ",\n";
        out << "    " << jstr("collisions")    << ": " << r.collisions         << ",\n";
        out << "    " << jstr("rehashes")     << ": " << r.rehashes           << ",\n";
        out << "    " << jstr("hash_capacity")<< ": " << r.hash_capacity      << ",\n";
        out << "    " << jstr("load_factor")  << ": " << jf(r.load_factor)    << ",\n";
        out << "    " << jstr("heap_ops")     << ": " << r.heap_ops           << ",\n";
        out << "    " << jstr("time_ms")      << ": " << r.time_ms            << ",\n";
        out << "    " << jstr("memory_kb")    << ": " << r.memory_kb          << ",\n";
        out << "    " << jstr("is_file")      << ": " << (r.is_file?"true":"false") << ",\n";
        out << "    " << jstr("filename")     << ": " << jstr(r.filename)     << "\n";
        out << "  }" << (i + 1 < results.size() ? "," : "") << "\n";
    }
    out << "]\n";

    std::cerr << "Benchmark concluido: " << results.size() << " medicoes -> " << out_file << "\n";
    return 0;
}