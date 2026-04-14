#include "free_list_arena.hpp"
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <new>

static const size_t MIN_BLOCK_SIZE = sizeof(FreeListArena::Header) + 16;

FreeListArena::FreeListArena(size_t size)
    : start_(static_cast<char*>(std::malloc(size)))
    , capacity_(size)
    , freeList_(nullptr)
{
    if (!start_) throw std::bad_alloc();
    addToFreeList(start_, capacity_);
}

FreeListArena::~FreeListArena() {
    std::free(start_);
}

void FreeListArena::addToFreeList(void* ptr, size_t blockSize) {
    Header* block = static_cast<Header*>(ptr);
    block->size = blockSize - sizeof(Header);
    block->next = freeList_;
    freeList_ = block;
}

size_t FreeListArena::alignUp(size_t n, size_t alignment) {
    return (n + alignment - 1) & ~(alignment - 1);
}

void* FreeListArena::allocate(size_t bytes, size_t alignment) {
    Header* prev = nullptr;
    Header* curr = freeList_;

    while (curr) {
        uintptr_t dataStart = reinterpret_cast<uintptr_t>(curr) + sizeof(Header);
        size_t misalignment = (alignment - (dataStart % alignment)) % alignment;
        size_t needed = bytes + misalignment;

        if (curr->size >= needed) {
            if (prev)
                prev->next = curr->next;
            else
                freeList_ = curr->next;

            size_t remaining = curr->size - needed;
            if (remaining > MIN_BLOCK_SIZE) {
                Header* newFree = reinterpret_cast<Header*>(
                    reinterpret_cast<char*>(curr) + sizeof(Header) + needed);
                newFree->size = remaining - sizeof(Header);
                newFree->next = freeList_;
                freeList_ = newFree;
                curr->size = needed;
            }
            char* userPtr = reinterpret_cast<char*>(curr) + sizeof(Header) + misalignment;
            return userPtr;
        }
        prev = curr;
        curr = curr->next;
    }
    return nullptr;
}

void FreeListArena::deallocate(void* ptr) {
    if (!ptr) return;
    if (ptr < start_ || ptr >= start_ + capacity_) return;
    Header* block = reinterpret_cast<Header*>(static_cast<char*>(ptr) - sizeof(Header));
    block->next = freeList_;
    freeList_ = block;
    coalesce();
}

void FreeListArena::coalesce() {
    std::vector<Header*> blocks;
    for (Header* h = freeList_; h; h = h->next)
        blocks.push_back(h);
    if (blocks.empty()) return;

    std::sort(blocks.begin(), blocks.end(),
              [](Header* a, Header* b) { return a < b; });

    std::vector<Header*> merged;
    Header* cur = blocks[0];
    for (size_t i = 1; i < blocks.size(); ++i) {
        Header* nxt = blocks[i];
        char* curEnd = reinterpret_cast<char*>(cur) + sizeof(Header) + cur->size;
        if (curEnd == reinterpret_cast<char*>(nxt)) {
            cur->size += sizeof(Header) + nxt->size;
        } else {
            merged.push_back(cur);
            cur = nxt;
        }
    }
    merged.push_back(cur);

    freeList_ = nullptr;
    for (size_t i = 0; i < merged.size(); ++i) {
        merged[i]->next = (i + 1 < merged.size()) ? merged[i + 1] : nullptr;
    }
    if (!merged.empty()) {
        freeList_ = merged[0];
    }
}


void FreeListArena::reset() {
    freeList_ = nullptr;
    addToFreeList(start_, capacity_);
}

size_t FreeListArena::getFreeBlocksCount() const {
    size_t count = 0;
    for (Header* h = freeList_; h; h = h->next)
        ++count;
    return count;
}
