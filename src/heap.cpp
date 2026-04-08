#include "heap.hpp"
#include <stdexcept>
#include <algorithm>

// Constrói heap a partir de vetor usando heapify O(n)
MaxHeap::MaxHeap(std::vector<HeapNode> data) : data_(std::move(data)) {
    if (data_.size() <= 1) return;
    // Começa pelo último nó interno e desce todos
    size_t start = (data_.size() - 2) / 2;
    for (int i = static_cast<int>(start); i >= 0; --i)
        sift_down(static_cast<size_t>(i));
}

void MaxHeap::push(const std::string& word, int count) {
    data_.emplace_back(word, count);
    sift_up(data_.size() - 1);
}

void MaxHeap::pop() {
    if (data_.empty()) throw std::underflow_error("Heap vazio");
    std::swap(data_.front(), data_.back());
    data_.pop_back();
    if (!data_.empty()) sift_down(0);
}

const HeapNode& MaxHeap::top() const {
    if (data_.empty()) throw std::underflow_error("Heap vazio");
    return data_.front();
}

void MaxHeap::sift_up(size_t idx) {
    while (idx > 0) {
        size_t p = parent(idx);
        ++ops_;
        if (data_[idx] > data_[p]) {
            std::swap(data_[idx], data_[p]);
            idx = p;
        } else break;
    }
}

void MaxHeap::sift_down(size_t idx) {
    size_t n = data_.size();
    while (true) {
        size_t largest = idx;
        size_t l = left(idx);
        size_t r = right(idx);
        ++ops_;
        if (l < n && data_[l] > data_[largest]) largest = l;
        if (r < n && data_[r] > data_[largest]) largest = r;
        if (largest == idx) break;
        std::swap(data_[idx], data_[largest]);
        idx = largest;
    }
}

std::vector<HeapNode> MaxHeap::top_k(size_t k) {
    // Trabalha sobre cópia para não consumir o heap original
    MaxHeap copy(*this);
    std::vector<HeapNode> result;
    size_t limit = std::min(k, copy.data_.size());
    result.reserve(limit);
    for (size_t i = 0; i < limit; ++i) {
        result.push_back(copy.top());
        copy.pop();
    }
    ops_ += copy.ops_;
    return result;
}