#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace ra {

// A small dynamic array used instead of std::vector to satisfy the lab rules.
template <typename T>
class SimpleArray {
public:
    SimpleArray() : data_(nullptr), size_(0), capacity_(0) {}

    SimpleArray(std::initializer_list<T> items) : data_(nullptr), size_(0), capacity_(0) {
        reserve(items.size());
        for (const T& item : items) {
            pushBack(item);
        }
    }

    SimpleArray(const SimpleArray& other) : data_(nullptr), size_(0), capacity_(0) {
        reserve(other.size_);
        for (std::size_t index = 0; index < other.size_; ++index) {
            data_[index] = other.data_[index];
        }
        size_ = other.size_;
    }

    SimpleArray(SimpleArray&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~SimpleArray() {
        delete[] data_;
    }

    SimpleArray& operator=(const SimpleArray& other) {
        if (this == &other) {
            return *this;
        }

        SimpleArray copy(other);
        swap(copy);
        return *this;
    }

    SimpleArray& operator=(SimpleArray&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    void reserve(std::size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        T* new_data = new T[new_capacity];
        for (std::size_t index = 0; index < size_; ++index) {
            new_data[index] = std::move(data_[index]);
        }

        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void pushBack(const T& value) {
        ensureCapacity();
        data_[size_] = value;
        ++size_;
    }

    void pushBack(T&& value) {
        ensureCapacity();
        data_[size_] = std::move(value);
        ++size_;
    }

    void append(const SimpleArray& other) {
        reserve(size_ + other.size_);
        for (std::size_t index = 0; index < other.size_; ++index) {
            data_[size_] = other.data_[index];
            ++size_;
        }
    }

    void clear() {
        size_ = 0;
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }

    T& operator[](std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("SimpleArray index is out of range");
        }
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("SimpleArray index is out of range");
        }
        return data_[index];
    }

    T* begin() {
        return data_;
    }

    const T* begin() const {
        return data_;
    }

    T* end() {
        return data_ + size_;
    }

    const T* end() const {
        return data_ + size_;
    }

    void swap(SimpleArray& other) noexcept {
        T* temp_data = data_;
        data_ = other.data_;
        other.data_ = temp_data;

        std::size_t temp_size = size_;
        size_ = other.size_;
        other.size_ = temp_size;

        std::size_t temp_capacity = capacity_;
        capacity_ = other.capacity_;
        other.capacity_ = temp_capacity;
    }

private:
    void ensureCapacity() {
        if (size_ < capacity_) {
            return;
        }

        const std::size_t new_capacity = capacity_ == 0 ? 4 : capacity_ * 2;
        reserve(new_capacity);
    }

    T* data_;
    std::size_t size_;
    std::size_t capacity_;
};

}  // namespace ra
