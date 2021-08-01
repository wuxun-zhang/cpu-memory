#include "ring_buffer.hpp"

int main() {
    // a float ring buffer with 5 elements
    ring_buffer::RingBuffer<float, 5> rb;
    rb.push_back(0);
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.push_back(4);
    rb.display();
    return 0;
}