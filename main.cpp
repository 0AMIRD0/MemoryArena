//
//  main.cpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#include <iostream>
#include <vector>
#include <memory_resource>
#include "linear_arena.hpp"
#include "free_list_arena.hpp"
#include "pool_allocator.hpp"
#include "arena_resource.hpp"
#include "visualization.hpp"

int main() {

    // 1. Linear Arena
    std::cout << "1. LinearArena (1024 bytes)\n";
    LinearArena linear(1024);
    int* a = static_cast<int*>(linear.allocate(sizeof(int)));
    double* b = static_cast<double*>(linear.allocate(sizeof(double), alignof(double)));
    *a = 42;
    *b = 3.1415;
    std::cout << "   stored: int=" << *a << ", double=" << *b << std::endl;
    visualizeLinearArena(linear);
    linear.reset();
    visualizeLinearArena(linear);
    std::cout << std::endl;

    // 2. Free List Arena
    std::cout << "2. FreeListArena (2048 bytes)\n";
    FreeListArena freeList(2048);
    void* p1 = freeList.allocate(100);
    void* p2 = freeList.allocate(200);
    std::cout << "   allocated: " << p1 << ", " << p2 << std::endl;
    visualizeFreeListArena(freeList);
    freeList.deallocate(p1);
    void* p3 = freeList.allocate(50);
    std::cout << "   after deallocate, new alloc: " << p3 << std::endl;
    freeList.reset();
    std::cout << "   after reset, allocate 300: " << freeList.allocate(300) << std::endl;
    visualizeFreeListArena(freeList);
    std::cout << std::endl;

    // 3. Pool Allocator
    std::cout << "3. PoolAllocator (64-byte blocks, 5 blocks)\n";
    PoolAllocator pool(64, 5);
    void* pa = pool.allocate();
    void* pb = pool.allocate();
    void* pc = pool.allocate();
    std::cout << "   allocated: " << pa << ", " << pb << ", " << pc << std::endl;
    visualizePoolAllocator(pool);
    pool.deallocate(pb);
    void* pd = pool.allocate();
    std::cout << "   after deallocate(pb), new: " << pd
              << (pd == pb ? " (reused)" : "") << std::endl;
    visualizePoolAllocator(pool);
    std::cout << std::endl;

    // 4. std::pmr integration
    std::cout << "4. std::pmr::vector with LinearArenaResource (4096 bytes)\n";
    LinearArenaResource resource(4096);
    std::pmr::polymorphic_allocator<int> alloc(&resource);
    std::pmr::vector<int> vec(alloc);
    for (int i = 0; i < 10; ++i)
        vec.push_back(i * i);
    std::cout << "   vector elements: ";
    for (int v : vec) std::cout << v << ' ';
    std::cout << std::endl;

    return 0;
}
