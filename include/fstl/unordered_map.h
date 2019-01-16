#pragma once

#ifndef FSTL_UNORDERED_MAP_H
#define FSTL_UNORDERED_MAP_H

#include "fstl/detail/erased_allocator.h"
#include "fstl/detail/erased_compare.h"
#include "fstl/utility.h"
#include "fstl/functional/hash.h"

namespace fstl {


namespace detail {
struct friendly_forward_list_base;
template <class T> struct hash;


template <class Hash, class Value>
struct erased_key_equal;

template <template<class> typename KeyEqual, typename Key, typename Value>
struct erased_key_equal<KeyEqual<Key>, Value> : erased_compare_base, KeyEqual<Key>
{
  using pair_type = fstl::pair<const Key, Value>;
  using base = KeyEqual<Key>;
  using base::base;

  erased_key_equal(const KeyEqual<Key> &ke) : base(ke) {}

  virtual bool compare_eq(const void *a, const void *b) override
  {
    return base::operator()(static_cast<const pair_type *>(a)->first,
      static_cast<const pair_type *>(b)->first);
  }
};

template <typename T>
struct equal_to
{
  bool operator()(const T &lhs, const T &rhs) { return lhs == rhs; }
};


template <class Alloc, class First, class Second>
struct erased_pair_allocator : erased_allocator<typename Alloc::template rebind<fstl::pair<First, Second>>::other>
{
  using value_type = fstl::pair<First, Second>;
  using base = erased_allocator<typename Alloc::template rebind<fstl::pair<First, Second>>::other>;
  using base::base;
  virtual void construct_pair_copy_default(void *pos, const void *first) override
  {
    ::new(pos) value_type{static_cast<const First &>(*static_cast<const First *>(first)),
                          Second{}};
  }
};

struct ll_node;

struct unordered_map_iterator_base {
  struct unordered_map_base const *m_umap = nullptr;
  ll_node *m_bucket_it = nullptr;
  size_t m_current_bucket = 0;

  void *data() const;
  unordered_map_iterator_base &next();

  bool operator ==(const unordered_map_iterator_base &other) const { return m_bucket_it == other.m_bucket_it; }
  bool operator !=(const unordered_map_iterator_base &other) const { return m_bucket_it != other.m_bucket_it; }
};

struct unordered_map_base {
  friend struct unordered_map_iterator_base;
public:
  using size_type = unsigned long;
  using iterator = unordered_map_iterator_base;


  unordered_map_base(size_type num_buckets,
    detail::erased_hash_base *hash,
    detail::erased_compare_base *key_eq,
    detail::erased_allocator_base *alloc);


  size_type bucket_count() const { return m_num_buckets; }
  size_type bucket_size(size_type bucket) const;

  size_type size() const { return m_size; }

  void clear();

protected:
  fstl::pair<iterator, bool> insert_copy(const void *key, const void *pair);

  void *at(const void *key);
  void *operator[](const void *key);
  size_type count(const void *key) const;
  iterator find(const void *key) const;
  iterator begin() const;
  iterator end() const { return {this, nullptr, m_num_buckets}; }

private:

  detail::friendly_forward_list_base *m_table;
  detail::erased_allocator_base *m_alloc;
  detail::erased_hash_base *m_hash;
  detail::erased_compare_base *m_equal;

  size_type m_size;
  size_type m_num_buckets;
};

} // end namespace detail

template <typename Key,
  typename Value,
  typename Hash = fstl::hash<Key>,
  typename KeyEqual = fstl::detail::equal_to<Key>,
  typename Allocator = fstl::detail::default_allocator<fstl::pair<const Key, Value>>
  >
class unordered_map : public detail::unordered_map_base
{
  using base = detail::unordered_map_base;
public:
  using key_equal = KeyEqual;
  using value_type = fstl::pair<const Key, Value>;

  class iterator : public detail::unordered_map_iterator_base {
    using base = unordered_map_iterator_base;
  public:
    iterator (const base &b) : base(b) {}
    value_type &operator *() {
      return *static_cast<value_type *>(base::data());
    }

    value_type *operator->() {
      return static_cast<value_type *>(base::data());
    }

    iterator &operator ++() {
      base::next(); return *this;
    }
  };

class const_iterator : public detail::unordered_map_iterator_base {
  using base = unordered_map_iterator_base;
public:
  const_iterator(const base &b) : base(b) {}
  const value_type &operator*() const {
    return *static_cast<const value_type *>(base::data());
  }

  const value_type *operator->() const {
    return static_cast<const value_type *>(base::data());
  }

  const_iterator &operator ++() {
    base::next();
    return *this;
  }
};

  explicit unordered_map( size_type bucket_count,
                          const Hash& hash = Hash(),
                          const key_equal& equal = key_equal(),
                          const Allocator& alloc = Allocator() )
  : unordered_map_base(
      bucket_count,
      new detail::erased_hash<Hash>(hash),
      new detail::erased_key_equal<KeyEqual, Value>(equal),
      new detail::erased_pair_allocator<Allocator, const Key, Value>(alloc))
  {
  }

  unordered_map() : unordered_map(100) {}

  Value &at(const Key &key) {
    return static_cast<value_type *>(base::at(&key))->second;
  }
  Value &operator[](const Key &key) {
    return static_cast<value_type *>(base::operator[](&key))->second;
  }
  size_type count (const Key &key) const { return base::count(&key); }
  iterator find(const Key &key) { return base::find(&key); }
  const_iterator find(const Key &key) const { return base::find(&key); }
  fstl::pair<iterator, iterator> equal_range(const Key &key)
  {
    iterator it = base::find(&key);
    return {it, ++it};
  }

  iterator begin() { return base::begin(); }
  iterator end() { return base::end(); }

  const_iterator begin() const { return base::begin(); }
  const_iterator end() const  { return base::end(); }

  fstl::pair<iterator, bool> insert(const value_type &val) {
    auto [it, ok] = base::insert_copy(&val.first, &val);
    return {iterator{it}, ok};
  }

  template <class ...Args>
  fstl::pair<iterator, bool> emplace(Args &... args) {
    return insert(value_type(std::forward<Args>(args)...));
  }
};

} // end namespace fstl

#endif //FSTL_UNORDERED_MAP_H
