#include <iostream>
#include "buddy_memory_pool.hpp"

int main() {
    buddy_mempool::MemoryPool::Instance().allocate(45);
    buddy_mempool::MemoryPool::Instance().allocate(10);
    buddy_mempool::MemoryPool::Instance().allocate(544);
    buddy_mempool::MemoryPool::Instance().allocate(500);

    buddy_mempool::MemoryPool::Instance().deallocate(64);
    buddy_mempool::MemoryPool::Instance().deallocate(32);
    return 0;
}
