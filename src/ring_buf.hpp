#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>

template <class T, std::size_t N> class ring_buf {
private:
  std::unique_ptr<T[]> data_;
  std::size_t size_{};
  std::size_t tail_{};
  std::size_t head_{};

public:
  ring_buf() : data_(std::make_unique<T[]>(N)) {}

  ring_buf(ring_buf &) = delete;
  ring_buf &operator=(const ring_buf &) = delete;

  ring_buf(ring_buf &&other) noexcept
      : data_(std::move(other.data_)), size_(std::exchange(other.size_, 0)),
        tail_(std::exchange(other.tail_, 0)),
        head_(std::exchange(other.head_, 0)) {}

  ring_buf &operator=(ring_buf &&other) noexcept {
    if (this != &other) {
      data_ = std::move(other.data_);
      size_ = std::exchange(other.size_, 0);
      tail_ = std::exchange(other.tail_, 0);
      head_ = std::exchange(other.head_, 0);
    }
    return *this;
  }

  ~ring_buf() = default;

  bool is_empty() const noexcept { return size_ == 0; }

  bool is_full() const noexcept { return size_ == N; }

  bool length() const noexcept { return size_; }

  bool size() const noexcept { return size_; }

  void push(const T &value) {
    if (is_full())
      throw std::runtime_error("Buffer is full");
    data_[tail_] = value;
    tail_ = (tail_ + 1) % N;
    ++size_;
  }

  void pop() {
    if (is_empty())
      throw std::runtime_error("Buffer is empty");
    head_ = (head_ + 1) % N;
    --size_;
  }

  const T &operator[](std::size_t n) const noexcept {
    return data_[(head_ + n) % N];
  }

  const T &at(std::size_t n) const {
    if (n >= size_)
      throw std::out_of_range("Invalid range");
    return data_[(head_ + n) % N];
  }

  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

  private:
    ring_buf *const container_;
    std::size_t virtual_index_;

  public:
    iterator(ring_buf *container, std::size_t virtual_index)
        : container_(container), virtual_index_(virtual_index) {}

    reference operator*() const {
      std::size_t physical_index = (container_->head_ + virtual_index_) % N;
      return container_->data_[physical_index];
    }

    pointer operator->() const { return &**this; }

    iterator &operator++() {
      virtual_index_++;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.container_ == b.container_ &&
             a.virtual_index_ == b.virtual_index_;
    }

    friend bool operator!=(const iterator &a, const iterator &b) {
      return !(a == b);
    }
  };

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size_); }
};
