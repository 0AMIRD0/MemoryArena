//
//  linear_arena.hpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#pragma once
#include <cstddef>

class LinearArena {
public:
    explicit LinearArena(size_t size);
    ~LinearArena();

    void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t));
    void reset();

    size_t used() const;
    size_t capacity() const;

private:
    char* start_;
    char* current_;
    size_t capacity_;
};
