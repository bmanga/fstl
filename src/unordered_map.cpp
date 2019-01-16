#include "fstl/unordered_map.h"
#include "fstl/forward_list.h"

#include "fstl/utility.h"

#include <cstdlib>
#include <new>
#include <stdexcept>
#include <fstl/unordered_map.h>


namespace fstl::detail {
struct friendly_forward_list_base : fstl::detail::forward_list_base
{
  friend class fstl::detail::unordered_map_base;
  friend struct fstl::detail::unordered_map_iterator_base;
};
using fstl::detail::friendly_forward_list_base;


unordered_map_base::unordered_map_base(unordered_map_base::size_type num_buckets, detail::erased_hash_base *hash,
                                       detail::erased_compare_base *key_eq, detail::erased_allocator_base *alloc)
  : m_table(new friendly_forward_list_base[num_buckets])
  , m_hash(hash)
  , m_equal(key_eq)
  , m_alloc(alloc)
  , m_size(0)
  , m_num_buckets(num_buckets)
{
  for (int j = 0; j < num_buckets; ++j) {
    m_table[j].set_allocator(m_alloc);
  }
}

fstl::pair<unordered_map_base::iterator, bool> unordered_map_base::insert_copy(const void *key, const void *pair) {
  auto bucket_idx = m_hash->hash(key) % m_num_buckets;
  auto &bucket = m_table[m_hash->hash(key) % m_num_buckets];
  // Check if container contains the key already.
  auto foundit = bucket.find(key, m_equal);
  if (*foundit != nullptr) {
    return {{this, foundit.m_node, bucket_idx}, false};
  }
  bucket.push_front_copy(pair);
  ++m_size;
  return {{this, bucket.first_node(), bucket_idx}, true};
}

void *unordered_map_base::at(const void *key) {
  auto bucket_idx = m_hash->hash(key) % m_num_buckets;
  auto &bucket = m_table[bucket_idx];
  auto data_it = bucket.find(key, m_equal);
  if (data_it != bucket.end()) {
    return *data_it;
  }
  throw std::out_of_range("unordered_map does not contain key");
}

void *unordered_map_base::operator[](const void *key) {
  auto bucket_idx = m_hash->hash(key) % m_num_buckets;
  auto &bucket = m_table[bucket_idx];
  auto data_it = bucket.find(key, m_equal);
  if (data_it != bucket.end()) {
    return *data_it;
  }
  // Default construct a value.
  // FIXME: This is very inefficient!
  auto *storage = m_alloc->allocate(1);
  m_alloc->construct_pair_copy_default(storage, key);
  bucket.push_front_copy(storage);
  ++m_size;
  m_alloc->destruct(storage);

  return bucket.front();
}

unordered_map_base::iterator unordered_map_base::begin() const
{
  for (size_type j = 0; j < m_num_buckets; ++j) {
    auto &bucket = m_table[j];
    if (bucket.empty()) continue;
    return {this, bucket.first_node(), j};
  }
  return end();
}

unordered_map_base::size_type unordered_map_base::count(const void *key) const {
  auto bucket_idx = m_hash->hash(key) % m_num_buckets;
  auto &bucket = m_table[bucket_idx];
  auto data_it = bucket.find(key, m_equal);
  if (data_it != bucket.end()) {
    return 1;
  }
  return 0;
}

unordered_map_base::iterator unordered_map_base::find(const void *key) {
  auto bucket_idx = m_hash->hash(key) % m_num_buckets;
  auto &bucket = m_table[bucket_idx];
  auto data_it = bucket.find(key, m_equal);
  if (data_it != bucket.end()) {
    return {this, data_it.m_node, bucket_idx};
  }
  return {this, nullptr, m_num_buckets};
}

unordered_map_base::size_type unordered_map_base::bucket_size(unordered_map_base::size_type bucket_idx) const {
  auto &bucket = m_table[bucket_idx];
  size_type num_elems = 0;
  for (const auto &_ : bucket) {
    ++num_elems;
  }
  return num_elems;
}

void unordered_map_base::clear() {
  for (size_type j = 0; j < m_num_buckets; ++j) {
    m_table[j].clear();
  }
  m_size = 0;
}

void *unordered_map_iterator_base::data() const {
  return *fstl::detail::forward_list_iterator_base(m_bucket_it);
}

unordered_map_iterator_base &unordered_map_iterator_base::next() {
  m_bucket_it = (++forward_list_iterator_base(m_bucket_it)).m_node;

  if (m_bucket_it == nullptr) {
    // We are at the end of the current bucket. Go to the next if available.
    detail::forward_list_base *fl = nullptr;
    while (++m_current_bucket < m_umap->bucket_count()) {
      auto &bucket = m_umap->m_table[m_current_bucket];
      if (!bucket.empty()) { m_bucket_it = bucket.first_node(); return *this; }
    }
  }
  return *this;
}
}



