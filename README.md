# vrt
Vrt is a thread-safe prefix tree with low memory usage and high read-write efficiency.

# Features
* The value can be of any type.
* In read-only or single-write, multi-read scenarios, you can set the template parameter kWriteLock to false to maximize performance.
* Lock-free, thread safety is achieved using techniques such as atomic operations and memory barriers.
* Using Epoch Based Reclamation to address cache ping-pong and false sharing issues during reads.

# Limitations
* The size of the key must be within 2 to the power of 20. However, this is generally sufficient for most use cases.
* Key must be string.
* You must anticipate the maximum number of read threads in advance, and set the template parameter kReadThreadNum to the corresponding number of read threads, otherwise it will cause an error. It is recommended to set it equal to the maximum number of read threads, as setting it too large will affect the performance of write threads.


# Building
This library only run in c++17 environment.

You just need to include all the code from the "include" directory into your project, and it will be ready to run.

If you want to generate and run unit_test or benchmark，you can use CMake as follow.

    mkdir build
    cd build
    cmake ..
    make
    ./test_vrt_node
    ./test_vrt
    ./vrt_benchmark
