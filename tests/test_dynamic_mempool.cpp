#include <iostream>

#include "include/dynamic_memory_pool.hpp"

int main() {
    void *ptr1 = dynamic_mempool::MemoryPool::Instance().allocate(10);
    void *ptr2 = dynamic_mempool::MemoryPool::Instance().allocate(80);
    void *ptr3 = dynamic_mempool::MemoryPool::Instance().allocate(150);

    dynamic_mempool::MemoryPool::Instance().deallocate(ptr2);
    dynamic_mempool::MemoryPool::Instance().deallocate(ptr1);
    return 0;
}
