#pragma once

#ifndef FSTL_FUNCTIONAL_HASH_H
#define FSTL_FUNCTIONAL_HASH_H

namespace fstl {
using size_t = unsigned long;

namespace detail {
size_t SuperFastHash(const char *data, int len);
}
template <class T>
struct hash
{
  size_t operator()(const T &val)
  {
    if constexpr (sizeof(T) <= sizeof(size_t))
      return reinterpret_cast<size_t>(reinterpret_cast<const T *>(val));
    else
      return detail::SuperFastHash(reinterpret_cast<const char *>(&val), sizeof(T));
  }
};

namespace detail {
struct erased_hash_base { virtual size_t hash(const void * val) = 0; };

template <class T> struct erased_hash;

template<template <class> class Hash, class T>
struct erased_hash<Hash<T>> : erased_hash_base
{
  erased_hash(const Hash<T> &hash) : m_hash(hash) {}
  virtual size_t hash(const void *val) override { return m_hash(*static_cast<const T *>(val)); }

  Hash<T> m_hash;
};
}

}
#endif //FSTL_FUNCTIONAL_HASH_H
