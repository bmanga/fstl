#pragma once

#ifndef FSTL_FORWARD_LIST_H
#define FSTL_FORWARD_LIST_H

#include "fstl/detail/erased_allocator.h"

namespace fstl {
namespace detail {

struct ll_node;
struct erased_compare_base;

struct forward_list_iterator_base
{
  forward_list_iterator_base(ll_node *node) : m_node(node) {}
  forward_list_iterator_base(const forward_list_iterator_base &) = default;
  void *operator*();
  forward_list_iterator_base &operator++();
  bool operator ==(const forward_list_iterator_base &other) const { return m_node == other.m_node; }
  bool operator !=(const forward_list_iterator_base &other) const { return m_node != other.m_node; }
  ll_node *m_node;
};

struct forward_list_base {
  using size_type = unsigned long;
  using iterator = forward_list_iterator_base;
  using const_iterator = forward_list_iterator_base; //TODO

public:
  forward_list_base() = default;
  explicit forward_list_base(erased_allocator_base *alloc)
    : m_alloc(alloc) {}

  explicit forward_list_base(size_type count, erased_allocator_base *alloc);

  void pop_front();
  void erase_after(const_iterator pos);

  bool empty() const { return m_first == nullptr; }

  void clear();
protected:
  void set_allocator(erased_allocator_base *alloc) { m_alloc = alloc; }
  void push_front_copy(const void *val);
  void push_front_move(void *val);
  void push_front_default();
  void push_front_constructed(void *val);
  void *front() const;
  iterator find(const void *cmp, erased_compare_base *comparator);
  iterator begin() { return {m_first}; }
  iterator end() { return {nullptr}; }
  ll_node *first_node() { return m_first; }
private:
  ll_node *m_first = nullptr;
  erased_allocator_base *m_alloc = nullptr;
};

} // end namespace detail

template <typename T, typename Allocator = detail::default_allocator<T>>
class forward_list : public detail::forward_list_base
{
  using base = detail::forward_list_base;

  struct forward_list_iterator : detail::forward_list_iterator_base
  {
    using base = detail::forward_list_iterator_base;
    using base::base;
    T &operator*() { return *static_cast<T *>(base::operator *());}
    forward_list_iterator &operator++() { base::operator ++(); return *this; }
  };

public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using iterator = forward_list_iterator;

public:
  forward_list() : base(new fstl::detail::erased_allocator(Allocator())) {}
  explicit forward_list(size_type count, Allocator alloc = Allocator())
    : base(count, new fstl::detail::erased_allocator(alloc)) {}

  void push_front(const T &val) { base::push_front_copy(&val); }
  void push_front(T &&val) { base::push_front_move(&val); }
  reference front() { return *static_cast<pointer>(base::front()); }
  const_reference front() const { return *static_cast<const_pointer>(base::front()); }

  iterator begin() { return {base::first_node()}; }
  iterator end() { return {nullptr}; }
};
} // end namespace fstl

#endif //FSTL_FORWARD_LIST_H
