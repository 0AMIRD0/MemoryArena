//
//  free_list_arena.hpp.hpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#pragma once
#include <cstddef>

class FreeListArena {
public:
    explicit FreeListArena(size_t size);
    ~FreeListArena();

    void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t));
    void deallocate(void* ptr);
    void reset();

    size_t getFreeBlocksCount() const;
    struct Header {
        Header* next;
        size_t size;
    };
    void coalesce();
private:
    char* start_;
    size_t capacity_;
    Header* freeList_;

    void addToFreeList(void* ptr, size_t blockSize);
    static size_t alignUp(size_t n, size_t alignment);
};
