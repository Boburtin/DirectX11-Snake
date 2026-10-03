#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>

template <auto V>
concept LOGBASE2_N_IS_NATURAL_NUM = (V > 0) && ((V & (V - 1)) == 0);

template <class T, std::size_t N>
    requires LOGBASE2_N_IS_NATURAL_NUM<N>
class RingBuffer
{
  private:
    std::unique_ptr<T[]> data_;
    std::size_t size_{};
    std::size_t tail_{};
    std::size_t head_{};

  public:
    RingBuffer() : data_(std::make_unique<T[]>(N))
    {
    }

    // TODO: allow copy constructor and copy assignment, don't depend on unique_ptr
    RingBuffer(const RingBuffer &) = delete;
    RingBuffer &operator=(const RingBuffer &) = delete;

    RingBuffer(RingBuffer &&other) noexcept
        : data_(std::move(other.data_)), size_(std::exchange(other.size_, 0)),
          tail_(std::exchange(other.tail_, 0)), head_(std::exchange(other.head_, 0))
    {
    }

    RingBuffer &operator=(RingBuffer &&other) noexcept
    {
        if (this != &other)
        {
            data_ = std::move(other.data_);
            size_ = std::exchange(other.size_, 0);
            tail_ = std::exchange(other.tail_, 0);
            head_ = std::exchange(other.head_, 0);
        }
        return *this;
    }

    ~RingBuffer() = default;

    bool is_empty() const noexcept
    {
        return size_ == 0;
    }

    bool is_full() const noexcept
    {
        return size_ == N;
    }

    std::size_t length() const noexcept
    {
        return size_;
    }

    std::size_t size() const noexcept
    {
        return size_;
    }

    std::size_t cap() const noexcept
    {
        return N;
    }

    void push(const T &value)
    {
        if (is_full())
            throw std::runtime_error("Buffer is full");
        data_[tail_] = value;
        if constexpr (LOGBASE2_N_IS_NATURAL_NUM<N>)
            tail_ = (tail_ + 1) & (N - 1);
        else
            tail_ = (tail_ + 1) % N;
        ++size_;
    }

    void pop()
    {
        if (is_empty())
            throw std::runtime_error("Buffer is empty");
        if constexpr (LOGBASE2_N_IS_NATURAL_NUM<N>)
            head_ = (head_ + 1) & (N - 1);
        else
            head_ = (head_ + 1) % N;
        --size_;
    }

    const T &operator[](std::size_t n) const noexcept
    {
        if constexpr (LOGBASE2_N_IS_NATURAL_NUM<N>)
            return data_[(head_ + n) & (N - 1)];
        else
            return data_[(head_ + n) % N];
    }

    const T &at(std::size_t n) const
    {
        if (n >= size_)
            throw std::out_of_range("Invalid range");
        if constexpr (LOGBASE2_N_IS_NATURAL_NUM<N>)
            return data_[(head_ + n) & (N - 1)];
        else
            return data_[(head_ + n) % N];
    }

    class iterator
    {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

      private:
        RingBuffer *const container_;
        std::size_t virtual_index_;

      public:
        iterator(RingBuffer *container, std::size_t virtual_index)
            : container_(container), virtual_index_(virtual_index)
        {
        }

        reference operator*() const
        {
            std::size_t physical_index;
            if constexpr (LOGBASE2_N_IS_NATURAL_NUM<N>)
                physical_index = (container_->head_ + virtual_index_) & (N - 1);
            else
                physical_index = (container_->head_ + virtual_index_) % N;
            return container_->data_[physical_index];
        }

        pointer operator->() const
        {
            return &this->operator*();
        }

        iterator &operator++()
        {
            virtual_index_++;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator &a, const iterator &b)
        {
            return a.container_ == b.container_ && a.virtual_index_ == b.virtual_index_;
        }

        friend bool operator!=(const iterator &a, const iterator &b)
        {
            return !(a == b);
        }
    };

    iterator begin()
    {
        return iterator(this, 0);
    }
    iterator end()
    {
        return iterator(this, size_);
    }
};
