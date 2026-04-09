//
//  arena_resource.hpp
//  MemArena
//
//  Created by salvatore on 09.04.2026.
//
#pragma once
#include <memory_resource>
#include "linear_arena.hpp"

class LinearArenaResource : public std::pmr::memory_resource {
public:
    explicit LinearArenaResource(size_t size);
    void reset();

protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    LinearArena arena_;
};
