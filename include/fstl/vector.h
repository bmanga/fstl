#pragma once

#ifndef FSTL_VECTOR_H
#define FSTL_VECTOR_H

#ifdef FSTL_USE_STD_LIB
#include <vector>
namespace fstl {
  using std::vector;
}
#else

#include "detail/erased_allocator.h"

namespace fstl
{
using detail::erased_allocator_base;

struct vector_base
{
  using size_type = unsigned long;
  using difference_type = long;

  explicit vector_base(erased_allocator_base *alloc);

  vector_base(size_type count, erased_allocator_base *alloc);
  vector_base(size_type count, const void *val, erased_allocator_base *alloc);
  vector_base(vector_base &&other) noexcept;
  vector_base(const vector_base &other);

  vector_base &operator=(const vector_base &other);
  vector_base &operator=(vector_base &&other);

  ~vector_base();

  size_type size() const { return m_size; }
  size_type capacity() const { return m_capacity; }
  bool empty() const { return m_size == 0; }
  void resize(size_type count);
  void reserve(size_type count);
  void shrink_to_fit() { resize(m_size); }
  void clear() noexcept;
  void pop_back();

  // NOTE: This will allow swapping between unrelated template instantiations.
  void swap(vector_base &other) noexcept;

  // These are public for implementation purposes, but will be hidden in vector.
  void *data() const { return m_data; }
  erased_allocator_base *get_allocator() const { return m_alloc; }

protected:
  void *at(size_type pos) const;
  void *back() const;
  void push_back_copy(const void *val);
  void push_back_move(void *val);
  void assign(size_type count, const void *val);
  void *erase(const void *pos);
  void *erase(const void *begin, const void *end);
  void *insert_copy(const void *pos, const void *val);
  void *insert_move(const void *pos, void *val);
  void resize_copy(size_type count, const void *val);

private:
  erased_allocator_base *m_alloc;
  void *m_data;
  size_t m_capacity;
  size_t m_size;
};


template <typename T, typename Allocator = detail::default_allocator<T>>
class vector : public vector_base
{
  //TODO : reverse iterator
public:
  using value_type = T;
  using allocator_type = Allocator;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using iterator = pointer;
  using const_iterator = const_pointer;


  vector() : vector_base(new detail::erased_allocator<allocator_type> (Allocator())) {}

  explicit vector(size_type count, const Allocator &alloc = Allocator() )
    : vector_base(count, new detail::erased_allocator<allocator_type>(alloc))
  {
  }

  vector(size_type count, const T &val, const Allocator &alloc = Allocator())
    : vector_base(count, &val, new detail::erased_allocator<allocator_type>(alloc)) {}

  reference operator[](size_type pos)
  {
    return *static_cast<pointer>(vector_base::at(pos));
  }

  const_reference operator[](size_type pos) const
  {
    return *static_cast<const_pointer>(vector_base::at(pos));
  }

  reference at(size_type pos)
  {
    return *static_cast<pointer>(vector_base::at(pos));
  }

  const_reference at(size_type pos) const
  {
    return *static_cast<const_pointer>(vector_base::at(pos));
  }

  // allocator_type get_allocator() const { return allocator_type(); }

  void push_back(const T &value) { vector_base::push_back_copy(&value); }
  void push_back(T &&value) { vector_base::push_back_move(&value); }

  // We can't create in-place, so the best we can do is move-construct via push_back/insert
  template <typename ...Args>
  reference emplace_back(Args &&...args)
  {
    push_back(value_type{static_cast<Args&&>(args)...});
    return back();
  }

  template <typename ...Args>
  reference emplace(const_iterator pos, Args &&...args)
  {
    return *insert(pos, value_type{static_cast<Args&&>(args)...});
  }

  void assign(size_type count, const T &value) { vector_base::assign(count, &value); }

  T *data() noexcept { return static_cast<T *>(vector_base::data()); }
  const T *data() const noexcept { return static_cast<const T *>(vector_base::data()); }

  T &front() { return *static_cast<T *>(vector_base::data()); }
  const T &front() const { return *static_cast<const T *>(vector_base::data()); }

  T &back() { return *static_cast<T *>(vector_base::back()); }
  const T &back() const { return *static_cast<const T *>(vector_base::back()); }

  iterator erase(iterator pos) { return static_cast<T *>(vector_base::erase(pos)); }
  iterator erase(iterator begin, iterator end) { return static_cast<T *>(vector_base::erase(begin, end)); }
  iterator begin() { return static_cast<iterator>(vector_base::data()); }
  iterator end()   { return static_cast<iterator>(vector_base::back()) + 1; }

  const_iterator begin() const { return static_cast<const_iterator>(vector_base::data()); }
  const_iterator end() const   { return static_cast<const_iterator>(vector_base::back()) + 1; }

  iterator insert(const_iterator pos, const T &value) {
    return static_cast<iterator>(vector_base::insert_copy(pos, &value));
  }

  using vector_base::resize;
  void resize(size_type count, const value_type &val) {
    vector_base::resize_copy(count, &val);
  }

  iterator insert(const_iterator pos, T &&value) {
    return static_cast<iterator>(vector_base::insert_move(pos, &value));
  }
};

#endif //FSTL_USE_STD_LIB
}

#endif //FSTL_VECTOR_H
