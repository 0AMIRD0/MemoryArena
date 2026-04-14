
#include <benchmark/benchmark.h>
#include <vector>
#include <random>

#include "linear_arena.hpp"
#include "free_list_arena.hpp"
#include "pool_allocator.hpp"

static constexpr size_t ARENA_SIZE = 1024 * 1024 * 10;

struct ArenaFixture : benchmark::Fixture {
    void SetUp(const ::benchmark::State&) override {
        linearArena = new LinearArena(ARENA_SIZE);
        freeListArena = new FreeListArena(ARENA_SIZE);
        poolAllocator = new PoolAllocator(64, ARENA_SIZE / 64);
    }

    void TearDown(const ::benchmark::State&) override {
        delete linearArena;
        delete freeListArena;
        delete poolAllocator;
    }

    LinearArena*   linearArena;
    FreeListArena* freeListArena;
    PoolAllocator* poolAllocator;
};

BENCHMARK_DEFINE_F(ArenaFixture, LinearArena_Allocate_16B)(benchmark::State& state) {
    for (auto _ : state) {
        void* p = linearArena->Allocate(16);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, LinearArena_Allocate_16B);

BENCHMARK_DEFINE_F(ArenaFixture, LinearArena_Allocate_1KB)(benchmark::State& state) {
    for (auto _ : state) {
        void* p = linearArena->Allocate(1024);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, LinearArena_Allocate_1KB);

BENCHMARK_DEFINE_F(ArenaFixture, LinearArena_Allocate_1MB)(benchmark::State& state) {
    for (auto _ : state) {
        void* p = linearArena->Allocate(1024 * 1024);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, LinearArena_Allocate_1MB);

BENCHMARK_DEFINE_F(ArenaFixture, FreeListArena_AllocateFree_16B)(benchmark::State& state) {
    for (auto _ : state) {
        void* p = freeListArena->Allocate(16);
        benchmark::DoNotOptimize(p);
        freeListArena->Deallocate(p);
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, FreeListArena_AllocateFree_16B);

BENCHMARK_DEFINE_F(ArenaFixture, FreeListArena_AllocateFree_1KB)(benchmark::State& state) {
    for (auto _ : state) {
        void* p = freeListArena->Allocate(1024);
        benchmark::DoNotOptimize(p);
        freeListArena->Deallocate(p);
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, FreeListArena_AllocateFree_1KB);

BENCHMARK_DEFINE_F(ArenaFixture, PoolAllocator_AllocateFree_64B)(benchmark::State& state) {
    for (auto _ : state) {
        void* p = poolAllocator->Allocate(64);
        benchmark::DoNotOptimize(p);
        poolAllocator->Deallocate(p);
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, PoolAllocator_AllocateFree_64B);

static void BM_NewDelete_16B(benchmark::State& state) {
    for (auto _ : state) {
        auto p = new char[16];
        benchmark::DoNotOptimize(p);
        delete[] p;
    }
}
BENCHMARK(BM_NewDelete_16B);

static void BM_NewDelete_1KB(benchmark::State& state) {
    for (auto _ : state) {
        auto p = new char[1024];
        benchmark::DoNotOptimize(p);
        delete[] p;
    }
}
BENCHMARK(BM_NewDelete_1KB);

static void BM_MallocFree_16B(benchmark::State& state) {
    for (auto _ : state) {
        void* p = malloc(16);
        benchmark::DoNotOptimize(p);
        free(p);
    }
}
BENCHMARK(BM_MallocFree_16B);

static void BM_MallocFree_1KB(benchmark::State& state) {
    for (auto _ : state) {
        void* p = malloc(1024);
        benchmark::DoNotOptimize(p);
        free(p);
    }
}
BENCHMARK(BM_MallocFree_1KB);

static constexpr int BATCH_SIZE = 1000;

BENCHMARK_DEFINE_F(ArenaFixture, LinearArena_BatchReset)(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < BATCH_SIZE; ++i) {
            void* p = linearArena->Allocate(32);
            benchmark::DoNotOptimize(p);
        }
        linearArena->Reset();
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, LinearArena_BatchReset);

BENCHMARK_DEFINE_F(ArenaFixture, FreeListArena_BatchDealloc)(benchmark::State& state) {
    std::vector<void*> ptrs;
    ptrs.reserve(BATCH_SIZE);
    for (auto _ : state) {
        for (int i = 0; i < BATCH_SIZE; ++i) {
            ptrs.push_back(freeListArena->Allocate(32));
        }
        for (void* p : ptrs) {
            freeListArena->Deallocate(p);
        }
        ptrs.clear();
    }
}
BENCHMARK_REGISTER_F(ArenaFixture, FreeListArena_BatchDealloc);

static void BM_NewDelete_Batch(benchmark::State& state) {
    std::vector<char*> ptrs;
    ptrs.reserve(BATCH_SIZE);
    for (auto _ : state) {
        for (int i = 0; i < BATCH_SIZE; ++i) {
            ptrs.push_back(new char[32]);
        }
        for (char* p : ptrs) {
            delete[] p;
        }
        ptrs.clear();
    }
}
BENCHMARK(BM_NewDelete_Batch);

BENCHMARK_DEFINE_F(ArenaFixture, FreeListArena_RandomSizes)(benchmark::State& state) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDist(8, 256);
    std::uniform_int_distribution<int> actionDist(0, 1);
    std::vector<void*> activePtrs;

    for (auto _ : state) {
        if (actionDist(gen) == 0 || activePtrs.empty()) {
            size_t sz = sizeDist(gen);
            void* p = freeListArena->Allocate(sz);
            if (p) activePtrs.push_back(p);
        } else {
            size_t idx = gen() % activePtrs.size();
            freeListArena->Deallocate(activePtrs[idx]);
            activePtrs.erase(activePtrs.begin() + idx);
        }
    }
    for (void* p : activePtrs) freeListArena->Deallocate(p);
}
BENCHMARK_REGISTER_F(ArenaFixture, FreeListArena_RandomSizes);

BENCHMARK_MAIN();
