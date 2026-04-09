//
//  visualisation.cpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#include "visualization.hpp"
#include "linear_arena.hpp"
#include "free_list_arena.hpp"
#include "pool_allocator.hpp"
#include <iostream>
#include <iomanip>

void visualizeLinearArena(const LinearArena& arena, std::ostream& os) {
    size_t used = arena.used();
    size_t cap = arena.capacity();
    const int barLen = 50;
    size_t usedLen = used * barLen / cap;
    os << "LinearArena: used " << used << " / " << cap << " bytes [";
    for (int i = 0; i < barLen; ++i)
        os << (static_cast<size_t>(i) < usedLen ? '#' : '.');
    os << "]\n";
}

void visualizeFreeListArena(const FreeListArena& arena, std::ostream& os) {
    os << "FreeListArena: free blocks = " << arena.getFreeBlocksCount() << std::endl;
}

void visualizePoolAllocator(const PoolAllocator& pool, std::ostream& os) {
    size_t total = pool.getBlocksCount();
    size_t free = pool.getFreeBlocksCount();
    os << "PoolAllocator: total blocks = " << total
       << ", used = " << (total - free)
       << ", free = " << free << std::endl;
}
