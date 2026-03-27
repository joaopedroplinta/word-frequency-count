#pragma once
#include <string>
#include <vector>
#include <cstddef>

// ============================================================
// Max-Heap para extrair as k palavras mais frequentes
//
// Cada elemento armazena (palavra, contagem).
// A prioridade é a contagem — maior contagem = maior prioridade.
//
// Operações:
//   push(word, count)  — insere e sobe (sift-up)
//   pop()              — remove raiz e desce (sift-down)
//   top()              — peek na raiz
//   build(vector)      — heapify em O(n)
// ============================================================

struct HeapNode {
    std::string word;
    int count;
    HeapNode(const std::string& w, int c) : word(w), count(c) {}
    bool operator<(const HeapNode& o) const { return count < o.count; }
    bool operator>(const HeapNode& o) const { return count > o.count; }
};

class MaxHeap {
public:
    MaxHeap() = default;
    explicit MaxHeap(std::vector<HeapNode> data);

    void push(const std::string& word, int count);
    void pop();
    const HeapNode& top() const;

    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }

    // Retorna os k maiores elementos em ordem decrescente de contagem
    // (consome o heap)
    std::vector<HeapNode> top_k(size_t k);

    size_t ops() const { return ops_; } // número de comparações realizadas

private:
    std::vector<HeapNode> data_;
    mutable size_t ops_ = 0;

    void sift_up(size_t idx);
    void sift_down(size_t idx);

    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t left(size_t i)   const { return 2 * i + 1; }
    size_t right(size_t i)  const { return 2 * i + 2; }
};