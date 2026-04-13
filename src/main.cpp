#include "wordcount.hpp"
#include "rng.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iomanip>

static void usage(const char* prog) {
    std::cerr
        << "Uso: " << prog << " [opções]\n"
        << "\n"
        << "Opções:\n"
        << "  -f <arquivo>   Arquivo de texto de entrada (padrão: stdin)\n"
        << "  -k <n>         Exibir top-k palavras mais frequentes (padrão: 10)\n"
        << "  -h <1|2>       Função de hash: 1=djb2, 2=fnv1a (padrão: 1)\n"
        << "  -r <1|2>       Gerador aleatório: 1=LCG, 2=Xorshift (padrão: 1)\n"
        << "  -s <seed>      Seed para geração aleatória (padrão: 42)\n"
        << "  -n <n>         Número de palavras aleatórias a gerar (padrão: 10000)\n"
        << "  -c <n>         Capacidade inicial da tabela hash (padrão: 16384)\n"
        << "  --random       Gerar texto aleatório em vez de ler arquivo\n"
        << "  --stats        Exibir estatísticas de desempenho\n"
        << "  --help         Exibir esta mensagem\n";
}

int main(int argc, char* argv[]) {
    std::string filename  = "";
    size_t      top_k     = 10;
    int         hash_func = 1;
    int         rng_meth  = 1;
    uint64_t    seed      = 42;
    size_t      num_words = 10000;
    size_t      capacity  = 16384;
    bool        random_mode = false;
    bool        show_stats  = false;

    // Parse de argumentos
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if      (arg == "--help")   { usage(argv[0]); return 0; }
        else if (arg == "--random") { random_mode = true; }
        else if (arg == "--stats")  { show_stats = true; }
        else if (arg == "-f" && i + 1 < argc) { filename  = argv[++i]; }
        else if (arg == "-k" && i + 1 < argc) { top_k     = std::stoull(argv[++i]); }
        else if (arg == "-h" && i + 1 < argc) { hash_func = std::stoi(argv[++i]); }
        else if (arg == "-r" && i + 1 < argc) { rng_meth  = std::stoi(argv[++i]); }
        else if (arg == "-s" && i + 1 < argc) { seed      = std::stoull(argv[++i]); }
        else if (arg == "-n" && i + 1 < argc) { num_words = std::stoull(argv[++i]); }
        else if (arg == "-c" && i + 1 < argc) {
            long long c = std::stoll(argv[++i]);
            if (c <= 0) { std::cerr << "Erro: capacidade deve ser >= 1\n"; return 1; }
            capacity = static_cast<size_t>(c);
        }
        else { std::cerr << "Argumento desconhecido: " << arg << "\n"; usage(argv[0]); return 1; }
    }

    HashFunc hf = (hash_func == 2) ? HashFunc::FNV1A : HashFunc::DJB2;
    RNG::Method rm = (rng_meth == 2) ? RNG::Method::XORSHIFT : RNG::Method::LCG;

    std::cout << "Função de hash : " << (hf == HashFunc::DJB2 ? "djb2" : "fnv1a") << "\n";
    std::cout << "Gerador RNG    : " << (rm == RNG::Method::LCG ? "LCG" : "Xorshift64") << "\n";

    WordCounter wc(capacity, hf);

    if (random_mode) {
        std::cout << "Modo           : aleatório (seed=" << seed << ", n=" << num_words << ")\n";
        RNG rng(seed, rm);
        wc.count_from_random(num_words, rng);
    } else if (!filename.empty()) {
        std::cout << "Modo           : arquivo (" << filename << ")\n";
        std::ifstream file(filename);
        if (!file) { std::cerr << "Erro: não foi possível abrir '" << filename << "'\n"; return 1; }
        wc.count_from_stream(file);
    } else {
        std::cout << "Modo           : stdin\n";
        wc.count_from_stream(std::cin);
    }

    // Exibe top-k
    auto results = wc.top_k(top_k);
    std::cout << "\n--- Top " << top_k << " palavras mais frequentes ---\n";
    std::cout << std::left << std::setw(5) << "#"
              << std::setw(25) << "Palavra"
              << "Freq\n";
    std::cout << std::string(40, '-') << "\n";
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << std::left << std::setw(5) << (i + 1)
                  << std::setw(25) << results[i].word
                  << results[i].count << "\n";
    }

    if (show_stats) wc.print_stats();

    return 0;
}