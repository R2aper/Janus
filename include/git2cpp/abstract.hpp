#pragma once
#ifndef ABSTRACT_HPP
#define ABSTRACT_HPP

namespace Git {

// Abstract base class for all libgit2 wrappers
template <typename T, typename ResourceType, void (*FreeFunc)(ResourceType *)> class AbstractClass {
protected:
  ResourceType *_handle = nullptr;

  explicit AbstractClass(ResourceType *handle) : _handle(handle) {}

  // Release object
  void release() {
    if (_handle) {
      FreeFunc(_handle);
      _handle = nullptr;
    }
  }

public:
  // Disabling copy constructor and copy assignment
  AbstractClass(const AbstractClass &) = delete;
  AbstractClass &operator=(const AbstractClass &) = delete;

  // Rule for move
  AbstractClass(AbstractClass &&other) noexcept : _handle(other._handle) {
    other._handle = nullptr;
  }

  AbstractClass &operator=(AbstractClass &&other) noexcept {
    if (this != &other) {
      release();
      _handle = other._handle;
      other._handle = nullptr;
    }
    return *this;
  }

  ResourceType *ptr() const { return _handle; }

  virtual ~AbstractClass() { release(); }

  // CRTP: methods for derived classes
  T &derived() { return static_cast<T &>(*this); }
  const T &derived() const { return static_cast<const T &>(*this); }
};

} // namespace Git

#endif // ABSTRACT_HPP
