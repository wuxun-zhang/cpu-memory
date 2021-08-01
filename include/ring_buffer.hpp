// Implementation of ring/circular buffer
#ifndef RING_BUFFER_HPP_
#define RING_BUFFER_HPP_

#include <array>
#include <iostream>

// A data structure that uses a *single*, *fixed size* buffer.
//
// Leveraged from below guide:
//  https://users.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html#TOC1
namespace ring_buffer {

// Declaration of iterator of RingBuffer container
template <typename T, size_t N>
class RingIter;

// Key take-aways includes:
//  *
//  *
template <typename T, size_t N>
class RingBuffer {
    // make iterator class as a friend of RingBuffer container
    friend class RingIter<T, N>;

    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef RingIter<T, N> iterator;
    typedef const RingIter<T, N> const_iterator;

private:
    std::array<T, N> myBuffer_;
    // head (oldest)
    size_type myBegin_;
    // number of element in the buffer
    size_type mySize_;
    // capacity of the buffer
    size_type myCapacity_;
    // tail (newest)
    size_type myEnd_;

public:
    // default ctor
    RingBuffer() : myBegin_(0), myEnd_(0), mySize_(0), myCapacity_(N) {}

    // reference to the head of the buffer
    reference front() { return myBuffer_[myBegin_]; }
    // const reference to the head of the buffer
    const_reference front() const { return myBuffer_[myBegin_]; }
    // reference to the tail of the buffer
    reference back() { return myBuffer_[myEnd_]; }
    // reference to the tail of the buffer
    const_reference back() const { return myBuffer_[myEnd_]; }

    void clear();

    void push_back(const value_type &item);
    void push(const value_type &item) { push_back(item); }
    void pop_front();
    void pop() { pop_front(); }

    size_type size() const { return mySize_; }
    size_type capacity() const { return myCapacity_; }
    bool empty();
    bool full();

    reference operator[](size_type index);
    const_reference operator[](size_type index) const;
    // perform range check
    reference at(size_type index);
    const_reference at(size_type index) const;

    iterator begin();
    const_iterator cbegin() const;
    iterator end();
    const_iterator cend() const;

    // print out all of elements in the buffer
    void display() {
        for (size_t i = myBegin_; i < mySize_; ++i) {
            std::cout << myBuffer_[i & (myCapacity_ - 1)] << ", ";
        }
        std::cout << std::endl;
    }

private:
    void increment_head();
    void increment_tail();
};

// Definition of iterator
template <typename T, size_t N>
class RingIter {
private:
    // as a reference to the given RingBuffer instance
    RingBuffer<T, N> &myRingBuffer_;
    // to get mod on the size
    size_t myModSize_;
    size_t myIndex_;
    size_t myOffset_;

public:
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;

    RingIter(RingBuffer<T, N> &rb, size_t size)
        : myRingBuffer_(rb), myModSize_(size - 1), myIndex_(0), myOffset_(0) {}

    bool operator==(const RingIter &other) {
        if (this == &other) return true;
        return (&myRingBuffer_ == &other.myRingBuffer_)
                && (myIndex_ + myOffset_ == other.myIndex_ + other.myOffset_);
    }
    bool operator!=(const RingIter &other) { return !operator==(other); }
    reference operator*() {
        return myRingBuffer_[(myOffset_ + myIndex_) & myModSize_];
    }
    // reference operator[](size_type idx) {
    //     return myRingBuffer_[(myOffset_ + myIndex_) & myModSize_];
    // }
    // prefix ++
    RingIter &operator++() {
        myIndex_++;
        return *this;
    }
    // suffix ++
    RingIter operator++(int) {
        RingIter copy = *this;
        this->myIndex_++;
        return copy;
    }
    // prefix --
    RingIter &operator--() {
        myIndex_--;
        return *this;
    }
    // suffix --
    RingIter operator--(int) {
        RingIter copy = *this;
        this->myIndex_--;
        return copy;
    }
};

} // namespace ring_buffer
#endif
