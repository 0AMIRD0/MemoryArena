#include "arena_resource.hpp"
#include <new>

LinearArenaResource::LinearArenaResource(size_t size)
    : arena_(size)
{}

void LinearArenaResource::reset() {
    arena_.reset();
}

void* LinearArenaResource::do_allocate(size_t bytes, size_t alignment) {
    void* p = arena_.allocate(bytes, alignment);
    if (!p) throw std::bad_alloc();
    return p;
}

void LinearArenaResource::do_deallocate(void*, size_t, size_t) {
    // LinearArena не поддерживает индивидуальное освобождение
}

bool LinearArenaResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}
