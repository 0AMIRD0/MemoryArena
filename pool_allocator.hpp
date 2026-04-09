//
//  pool_allocator.hpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#pragma once
#include <cstddef>

class PoolAllocator {
public:
    PoolAllocator(size_t blockSize, size_t blocksCount);
    ~PoolAllocator();

    void* allocate();
    void deallocate(void* ptr);

    size_t getBlockSize() const;
    size_t getBlocksCount() const;
    size_t getFreeBlocksCount() const;

private:
    char* start_;
    void* freeList_;
    size_t blockSize_;
    size_t blocksCount_;
    size_t freeCount_;
};
