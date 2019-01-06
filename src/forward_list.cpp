#include "fstl/forward_list.h"
#include "fstl/detail/erased_compare.h"

#include <cstdlib>
#include <fstl/forward_list.h>


namespace fstl::detail {
struct ll_node
{
  ll_node *next = nullptr;
  void *data = nullptr;
};

forward_list_iterator_base &forward_list_iterator_base::operator ++()
{
  m_node = m_node->next;
  return *this;
}


void *forward_list_iterator_base::operator *() { return m_node->data; }
}

using fstl::detail::ll_node,
      fstl::detail::forward_list_base;

namespace {
ll_node *create(fstl::detail::erased_allocator_base *alloc)
{
  ll_node *node = ::new ll_node;
  node->data = alloc->allocate(1);
  return node;
}

void destroy(ll_node *node, fstl::detail::erased_allocator_base *alloc)
{
  alloc->destruct(node->data);
  alloc->deallocate(node->data, 1);
  node->next = nullptr;
  delete node;
}
}

forward_list_base::forward_list_base(size_t count, erased_allocator_base *alloc)
  : m_alloc(alloc) {
  auto elem_size = m_alloc->element_size();
  ll_node *iter = m_first = create(m_alloc);
  for (size_t j = 0; j < count; ++j) {
    iter->next = create(m_alloc);
    iter = iter->next;
  }
}

void fstl::detail::forward_list_base::push_front_copy(const void *val) {
  ll_node *new_node = create(m_alloc);
  m_alloc->construct_copy(new_node->data, val);
  new_node->next = m_first;
  m_first = new_node;
}

void fstl::detail::forward_list_base::push_front_move(void *val) {
  ll_node *new_node = create(m_alloc);
  m_alloc->construct_move(new_node->data, val);
  new_node->next = m_first;
  m_first = new_node;
}


void fstl::detail::forward_list_base::push_front_default() {
  ll_node *new_node = create(m_alloc);
  m_alloc->construct(new_node->data);
  new_node->next = m_first;
  m_first = new_node;
}

void fstl::detail::forward_list_base::push_front_constructed(void *val) {
  ll_node *new_node = create(m_alloc);
  new_node->data = val;
  new_node->next = m_first;
  m_first = new_node;
}

void fstl::detail::forward_list_base::erase_after(forward_list_base::const_iterator pos) {
  ll_node *to_erase = pos.m_node->next;
  pos.m_node->next = to_erase->next;
  m_alloc->destruct(to_erase->data);
  to_erase->~ll_node();
  free(to_erase);
}

void fstl::detail::forward_list_base::pop_front() {
  auto *old_first = m_first;
  m_first = m_first->next;
  m_alloc->destruct(old_first->data);
  old_first->~ll_node();
  free(old_first);
}

void *fstl::detail::forward_list_base::front() const { return m_first->data; }

forward_list_base::iterator
fstl::detail::forward_list_base::find(const void *cmp, fstl::detail::erased_compare_base *comparator) {
  for (auto *it = m_first; it != nullptr; it = it->next) {
    if (comparator->compare_eq(it->data, cmp)) {
      return it;
    }
  }
  return {nullptr};
}

void fstl::detail::forward_list_base::clear() {
  ll_node *it = m_first;
  ll_node *next = nullptr;
  while (it != nullptr) {
    next = it->next;
    destroy(it, m_alloc);
    it = next;
  }
  m_first = nullptr;
}



