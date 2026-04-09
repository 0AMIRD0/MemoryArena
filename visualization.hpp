//
//  visualization.hpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#pragma once
#include <iosfwd>
#include <iostream>
class LinearArena;
class FreeListArena;
class PoolAllocator;

void visualizeLinearArena(const LinearArena& arena, std::ostream& os = std::cout);
void visualizeFreeListArena(const FreeListArena& arena, std::ostream& os = std::cout);
void visualizePoolAllocator(const PoolAllocator& pool, std::ostream& os = std::cout);
