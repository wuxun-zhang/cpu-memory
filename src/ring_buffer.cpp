#include "ring_buffer.hpp"

namespace ring_buffer {

template <typename T, size_t N>
void RingBuffer<T, N>::clear() {
    myBegin_ = 1;
    myEnd_ = 0;
    mySize_ = 0;
}

template <typename T, size_t N>
void RingBuffer<T, N>::push_back(const value_type &item) {
    increment_tail();
    if (full()) increment_head();
    myBuffer_[myEnd_] = item;
}

template <typename T, size_t N>
void RingBuffer<T, N>::pop_front() {
    increment_head();
}

template <typename T, size_t N>
bool RingBuffer<T, N>::empty() {
    return mySize_ == 0;
}

template <typename T, size_t N>
bool RingBuffer<T, N>::full() {
    return mySize_ == myCapacity_;
}

template <typename T, size_t N>
typename RingBuffer<T, N>::reference RingBuffer<T, N>::operator[](
        size_type index) {
    return myBuffer_[(myBegin_ + index) & (myCapacity_ - 1)];
}

template <typename T, size_t N>
typename RingBuffer<T, N>::const_reference RingBuffer<T, N>::operator[](
        size_type index) const {
    return myBuffer_[(myBegin_ + index) & (myCapacity_ - 1)];
}

template <typename T, size_t N>
typename RingBuffer<T, N>::reference RingBuffer<T, N>::at(size_type index) {
    if (index < mySize_)
        return myBuffer_[(myBegin_ + index) & (myCapacity_ - 1)];
    throw std::out_of_range("index too large.");
}

template <typename T, size_t N>
typename RingBuffer<T, N>::const_reference RingBuffer<T, N>::at(
        size_type index) const {
    if (index < mySize_)
        return myBuffer_[(myBegin_ + index) & (myCapacity_ - 1)];
    throw std::out_of_range("index too large.");
}

template <typename T, size_t N>
typename RingBuffer<T, N>::iterator RingBuffer<T, N>::begin() {
    iterator iter(*this, myCapacity_);
    iter.myIndex_ = 0;
    iter.myOffset_ = myBegin_;
    return iter;
}

template <typename T, size_t N>
typename RingBuffer<T, N>::const_iterator RingBuffer<T, N>::cbegin() const {
    const_iterator iter(*this, myCapacity_);
    iter.myIndex_ = 0;
    iter.myOffset_ = myBegin_;
    return iter;
}

template <typename T, size_t N>
typename RingBuffer<T, N>::iterator RingBuffer<T, N>::end() {
    iterator iter(*this, myCapacity_);
    iter.myIndex_ = mySize_;
    iter.myOffset_ = myBegin_;
    return iter;
}

template <typename T, size_t N>
typename RingBuffer<T, N>::const_iterator RingBuffer<T, N>::cend() const {
    const_iterator iter(*this, myCapacity_);
    iter.myIndex_ = 0;
    iter.myOffset_ = myBegin_;
    return iter;
}

template <typename T, size_t N>
void RingBuffer<T, N>::increment_head() {
    // if there is no element in the buffer, no need to increment head
    if (mySize_ == 0) return;
    ++myBegin_;
    --mySize_;
    // if size increases to capacity, then reset to 0
    if (myBegin_ == myCapacity_) myBegin_ = 0;
}

template <typename T, size_t N>
void RingBuffer<T, N>::increment_tail() {
    ++myEnd_;
    ++mySize_;
    if (myEnd_ == myCapacity_) myEnd_ = 0;
}

} // namespace ring_buffer
