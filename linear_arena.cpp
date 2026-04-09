//
//  linear_arena.cpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#include "linear_arena.hpp"
#include <cstdlib>
#include <cstdint>
#include <new>

LinearArena::LinearArena(size_t size)
    : start_(static_cast<char*>(std::malloc(size)))
    , current_(start_)
    , capacity_(size)
{
    if (!start_) throw std::bad_alloc();
}

LinearArena::~LinearArena() {
    std::free(start_);
}

void* LinearArena::allocate(size_t bytes, size_t alignment) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(current_);
    size_t misalignment = (alignment - (addr % alignment)) % alignment;
    size_t total = bytes + misalignment;
    if (current_ + total > start_ + capacity_)
        return nullptr;
    char* ptr = current_ + misalignment;
    current_ += total;
    return ptr;
}

void LinearArena::reset() {
    current_ = start_;
}

size_t LinearArena::used() const {
    return static_cast<size_t>(current_ - start_);
}

size_t LinearArena::capacity() const {
    return capacity_;
}
