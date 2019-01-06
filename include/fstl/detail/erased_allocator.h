#pragma once

#ifndef FSTL_ERASED_ALLOCATOR_H
#define FSTL_ERASED_ALLOCATOR_H

#include "fstl/type_traits.h"
#include <new>

extern "C" void* calloc( size_t num, size_t size ) noexcept;

namespace fstl {
using size_t = unsigned long;
namespace detail {
template<typename T>
struct default_allocator {
  using value_type = T;

  template< class U > struct rebind { using other = default_allocator<U>; };

  T *allocate(size_t n) { return (T*) ::operator new( n * sizeof (T)); }

  void deallocate(T *p, size_t n) { ::operator delete(p, n); }
};

struct erased_allocator_base {
  virtual erased_allocator_base *clone() = 0;

  virtual void *allocate(size_t n) = 0;

  virtual void deallocate(void *p, size_t n) = 0;

  virtual void construct(void *p) = 0;

  virtual void construct_copy(void *p, const void *val) = 0;

  virtual void construct_move(void *p, void *val) = 0;

  /* These are for the specialized pair allocators */
  virtual void construct_pair_copy_default(void *pos, const void *first) { (void)pos; (void)first; };

  virtual void destruct(void *p) = 0;

  virtual size_t element_size() const = 0;

  virtual ~erased_allocator_base() {};
};

template<typename Alloc>
struct erased_allocator : public erased_allocator_base {
  using value_type = typename Alloc::value_type;

  virtual erased_allocator_base *clone() {
    return new erased_allocator{allocator};
  }

  erased_allocator(const Alloc &alloc) : allocator(alloc) {}

  virtual void *allocate(size_t n) override { return allocator.allocate(n); }

  virtual void deallocate(void *p, size_t n) override { allocator.deallocate(static_cast<value_type *>(p), n); }

  virtual void construct(void *p) override {
    if constexpr(fstl::is_default_constructible<value_type>::value)
      ::new(p) value_type{};
  }

  virtual void destruct(void *p) override { static_cast<value_type *>(p)->~value_type(); }

  virtual void construct_copy(void *p, const void *val) override {
    ::new(p) value_type{*static_cast<const value_type *>(val)};
  }

  virtual void construct_move(void *p, void *val) override {
    ::new(p) value_type{static_cast<value_type &&>(*static_cast<value_type *>(val))};
  }

  virtual size_t element_size() const override { return sizeof(value_type); }

  Alloc allocator;
};
}
}

#endif //FSTL_ERASED_ALLOCATOR_H
