//
//  pool_allocator.cpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#include "pool_allocator.hpp"
#include <cstdlib>
#include <new>
#include <stdexcept>

PoolAllocator::PoolAllocator(size_t blockSize, size_t blocksCount)
    : start_(static_cast<char*>(std::malloc(blockSize * blocksCount)))
    , freeList_(start_)
    , blockSize_(blockSize)
    , blocksCount_(blocksCount)
    , freeCount_(blocksCount)
{
    if (!start_) throw std::bad_alloc();
    if (blockSize_ < sizeof(void*)) {
        std::free(start_);
        throw std::invalid_argument("blockSize must be >= sizeof(void*)");
    }
    if (blockSize_ % alignof(std::max_align_t) != 0) {
        std::free(start_);
        throw std::invalid_argument("blockSize must be multiple of alignof(std::max_align_t)");
    }

    char* cur = start_;
    for (size_t i = 0; i < blocksCount_ - 1; ++i) {
        char* nxt = cur + blockSize_;
        *reinterpret_cast<char**>(cur) = nxt;
        cur = nxt;
    }
    *reinterpret_cast<char**>(cur) = nullptr;
}

PoolAllocator::~PoolAllocator() {
    std::free(start_);
}


void* PoolAllocator::allocate() {
    if (!freeList_) return nullptr;
    void* ptr = freeList_;
    freeList_ = *static_cast<void**>(freeList_); // здесь static_cast допустим (void* -> void**)
    --freeCount_;
    return static_cast<char*>(ptr) + sizeof(void*);
}

void PoolAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    void** block = reinterpret_cast<void**>(static_cast<char*>(ptr) - sizeof(void*));
    *block = freeList_;
    freeList_ = block;
    ++freeCount_;
}

size_t PoolAllocator::getBlockSize() const {
    return blockSize_;
}

size_t PoolAllocator::getBlocksCount() const {
    return blocksCount_;
}

size_t PoolAllocator::getFreeBlocksCount() const {
    return freeCount_;
}
