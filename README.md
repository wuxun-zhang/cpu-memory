# cpu-memory

## Description

Demonstrate the effects of CPU memory on SW performance

## Build and test

~~~shell
mkdir build
cd build
cmake ..
make
ctest -V
~~~

## Samples

- [memory access](doc/memory_access.md)

- [cache bandwidth](doc/cache_bandwidth.md)

- [cache line](doc/cache_line.md)

- [spatial locality](doc/spatial_locality.md)

- [temporal locality](doc/temporal_locality.md)

- [dynamic memory pool example](tests/test_dynamic_mempool.cpp)
