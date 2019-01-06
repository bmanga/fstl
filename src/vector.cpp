#include "fstl/vector.h"

#include <stdexcept>
#include <fstl/vector.h>


static constexpr float GROWTH_FACTOR = 1.4f;


static void *ptr_at_idx(const fstl::vector_base &vec, fstl::size_t offset)
{
  return reinterpret_cast<char *>(vec.data()) + offset * vec.get_allocator()->element_size();
}


fstl::vector_base::vector_base(unsigned long count, erased_allocator_base *alloc)
  : m_alloc(alloc)
  , m_capacity(count)
  , m_size(count)
{
  m_data = m_alloc->allocate(count);

  for (size_type j = 0; j < count; ++j) {
    void *p = reinterpret_cast<char *>(m_data) + j * m_alloc->element_size();
    m_alloc->construct(p);
  }
}

fstl::vector_base::vector_base(fstl::vector_base::size_type count, const void *val,
                               fstl::erased_allocator_base *alloc)
  : m_alloc(alloc)
  , m_capacity(count)
  , m_size(count)
  {
  m_data = m_alloc->allocate(count);

  for (size_type j = 0; j < count; ++j) {
    void *p = reinterpret_cast<char *>(m_data) + j * m_alloc->element_size();
    m_alloc->construct_copy(p, val);
  }
}

fstl::vector_base::vector_base(fstl::vector_base &&other) noexcept {
  m_data = other.m_data;
  m_capacity = other.m_capacity;
  m_size = other.m_size;
  // We can't do any better than copying the allocator unfortunately.
  m_alloc = other.m_alloc->clone();

  other.m_data = nullptr;
  other.m_capacity = 0;
  other.m_size = 0;
}

fstl::vector_base::vector_base(const fstl::vector_base &other)
  : m_alloc(other.m_alloc->clone())
  , m_capacity(other.m_size)
  , m_size(other.m_size)
  {
  m_data = m_alloc->allocate(m_size);
  auto elem_size = m_alloc->element_size();
  for (size_type j = 0; j < m_size; ++j) {
    void *old_p = reinterpret_cast<char *>(other.m_data) + j * elem_size;
    void *new_p = reinterpret_cast<char *>(m_data) + j * elem_size;
    m_alloc->construct_copy(new_p, old_p);
  }
}

fstl::vector_base::vector_base(fstl::erased_allocator_base *alloc)
  : m_alloc(alloc)
  , m_data(nullptr)
  , m_capacity(0)
  , m_size(0){}

fstl::vector_base::~vector_base() { clear(); delete m_alloc; }

fstl::vector_base &fstl::vector_base::operator=(const fstl::vector_base &other) {
  clear();
  if (other.m_size > m_size) {
    resize(other.m_size);
  }
  for (size_type j = 0; j < other.size(); ++j) {
    void *old_p = reinterpret_cast<char *>(other.m_data) + j * m_alloc->element_size();
    void *new_p = reinterpret_cast<char *>(m_data) + j * m_alloc->element_size();
    m_alloc->construct_copy(new_p, old_p);
  }
  m_size = other.m_size;

  return *this;
}

fstl::vector_base &fstl::vector_base::operator=(fstl::vector_base &&other) {
  this->~vector_base();
  new (this) vector_base(static_cast<vector_base &&>(other));
  return *this;
}

void *fstl::vector_base::at(fstl::vector_base::size_type pos) const {
  if (pos >= m_size) throw std::out_of_range("vector index out of range");
  return reinterpret_cast<char *>(m_data) + pos * m_alloc->element_size();
}



void fstl::vector_base::push_back_copy(const void *val) {
  if (m_size == m_capacity) {
    reserve(m_capacity * GROWTH_FACTOR + 3);
  }
  m_alloc->construct_copy(ptr_at_idx(*this, m_size), val);
  ++m_size;
}

void fstl::vector_base::push_back_move(void *val)
{
  if (m_size == m_capacity) {
    reserve(m_capacity * GROWTH_FACTOR + 3);
  }

  m_alloc->construct_move(ptr_at_idx(*this, m_size), val);
  ++m_size;
}

void fstl::vector_base::resize(fstl::vector_base::size_type count)
{
  auto curr_size = m_size;

  if (count > m_capacity)
    reserve(count);

  for (auto j = count; j < curr_size; ++j) {
    m_alloc->destruct(ptr_at_idx(*this, j));
  }
  for (auto j = curr_size; j < count; ++j) {
    m_alloc->construct(ptr_at_idx(*this, j));
  }

  m_size = count;
}

void fstl::vector_base::resize_copy(fstl::vector_base::size_type count, const void *val) {
  auto curr_size = m_size;

  if (count > m_capacity)
    reserve(count);

  for (auto j = count; j < curr_size; ++j) {
    m_alloc->destruct(ptr_at_idx(*this, j));
  }
  for (auto j = curr_size; j < count; ++j) {
    m_alloc->construct_copy(ptr_at_idx(*this, j), val);
  }

  m_size = count;
}


void fstl::vector_base::clear() noexcept {
  for (size_type j = 0; j < m_size; ++j) {
    void *p = reinterpret_cast<char *>(m_data) + j * m_alloc->element_size();
    m_alloc->destruct(p);
  }
  m_size = 0;
}


void *fstl::vector_base::back() const {
  return ptr_at_idx(*this, m_size - 1);
}

void fstl::vector_base::pop_back() {
  m_alloc->destruct(back());
  --m_size;
}

void *fstl::vector_base::erase(const void *posit) {
  void *pos = const_cast<void *>(posit);
  m_alloc->destruct(pos);
  auto elem_size = m_alloc->element_size();
  char *current = static_cast<char *>(pos);
  char *next = static_cast<char *>(pos) + elem_size;
  char *end = static_cast<char *>(back()) + elem_size;
  while (next != end) {
    m_alloc->construct_move(current, next);
    current = next;
    next = next + elem_size;
  }
  --m_size;
  return pos;
}
void *fstl::vector_base::erase(const void *begin, const void *end) {
  auto elem_size = m_alloc->element_size();
  char *first = static_cast<char *>(const_cast<void *>(begin));
  char *last = static_cast<char *>(const_cast<void *>(end));
  char *vec_end = static_cast<char *>(back()) + elem_size;
  char *it = first;

  while (it != last) {
    m_alloc->destruct(it);
    --m_size;
    it += elem_size;
  }

  auto *next = it;
  it = first;

  while (next != vec_end) {
    m_alloc->construct_move(it, next);
    it = next;
    next = next + elem_size;
  }
  return it;
}


void *fstl::vector_base::insert_copy(const void *posit, const void *val) {
  auto *pos = const_cast<void *>(posit);
  auto *pos_it = static_cast<char *>(pos);
  auto *curr_begin = static_cast<char *>(data());
  auto elem_size = m_alloc->element_size();

  size_type pos_idx = (pos_it - curr_begin) / elem_size;

  auto *curr_data = m_data;
  size_t j = 0;
  ++m_size;
  if (m_size > m_capacity) {
    m_data = m_alloc->allocate(m_capacity * GROWTH_FACTOR + 3);
    for (; j < pos_idx; ++j) {
      void *old_p = reinterpret_cast<char *>(curr_data) + j * elem_size;
      void *new_p = reinterpret_cast<char *>(m_data) + j * elem_size;
      m_alloc->construct_move(new_p, old_p);
      m_alloc->destruct(old_p);
    }
    auto *insert_slot = reinterpret_cast<char *>(m_data) + j * elem_size;
    m_alloc->construct_copy(insert_slot, val);
    while (++j < m_size) {
      void *old_p = reinterpret_cast<char *>(curr_data) + (j - 1) * elem_size;
      void *new_p = reinterpret_cast<char *>(m_data) + j * elem_size;
      m_alloc->construct_move(new_p, old_p);
      m_alloc->destruct(old_p);
    }
    return insert_slot;
  } else {
    char *last = static_cast<char *>(back());
    char *prev = last - elem_size;
    while (prev != pos_it) {
      m_alloc->construct_move(last, prev);
      m_alloc->destruct(prev);
      last = prev;
      prev -= elem_size;
    }
    m_alloc->construct_copy(pos_it, val);
    return pos_it;
  }
}

void *fstl::vector_base::insert_move(const void *posit, void *val) {
  auto *pos = const_cast<void *>(posit);
  auto *pos_it = static_cast<char *>(pos);
  auto *curr_begin = static_cast<char *>(data());
  auto elem_size = m_alloc->element_size();

  size_type pos_idx = (pos_it - curr_begin) / elem_size;

  auto *curr_data = m_data;
  size_t j = 0;
  ++m_size;
  if (m_size > m_capacity) {
    m_data = m_alloc->allocate(m_capacity * GROWTH_FACTOR + 3);
    for (; j < pos_idx; ++j) {
      void *old_p = reinterpret_cast<char *>(curr_data) + j * elem_size;
      void *new_p = reinterpret_cast<char *>(m_data) + j * elem_size;
      m_alloc->construct_move(new_p, old_p);
      m_alloc->destruct(old_p);
    }
    auto *insert_slot = reinterpret_cast<char *>(m_data) + j * elem_size;
    m_alloc->construct_move(insert_slot, val);
    while (++j < m_size) {
      void *old_p = reinterpret_cast<char *>(curr_data) + (j - 1) * elem_size;
      void *new_p = reinterpret_cast<char *>(m_data) + j * elem_size;
      m_alloc->construct_move(new_p, old_p);
      m_alloc->destruct(old_p);
    }
    return insert_slot;
  } else {
    char *last = static_cast<char *>(back());
    char *prev = last - elem_size;
    while (prev != pos_it) {
      m_alloc->construct_move(last, prev);
      m_alloc->destruct(prev);
      last = prev;
      prev -= elem_size;
    }
    m_alloc->construct_move(pos_it, val);
    return pos_it;
  }
}


void fstl::vector_base::assign(fstl::vector_base::size_type count, const void *val) {
  auto *curr_data = m_data;
  if (count <= m_capacity) {
    for (size_type j = 0; j < count; ++j) {
      void *old_p = reinterpret_cast<char *>(curr_data) + j * m_alloc->element_size();
      m_alloc->destruct(old_p);
      m_alloc->construct_copy(old_p, val);
    }
  } else {
    *this = vector_base{count, val, m_alloc};
  }
}

void fstl::vector_base::reserve(fstl::vector_base::size_type count) {
  if (count <= m_capacity) {
    return;
  }
  m_capacity = count;
  auto curr_data = m_data;
  auto new_storage = m_data = m_alloc->allocate(m_capacity);
  for (size_type j = 0; j < m_size; ++j) {
    void *old_p = reinterpret_cast<char *>(curr_data) + j * m_alloc->element_size();
    void *new_p = reinterpret_cast<char *>(new_storage) + j * m_alloc->element_size();
    m_alloc->construct_move(new_p, old_p);
    m_alloc->destruct(old_p);
  }
}

void fstl::vector_base::swap(fstl::vector_base &other) noexcept {
  void *tmp_data = m_data;
  m_data = other.m_data;
  other.m_data = tmp_data;

  auto tmp_size = m_size;
  m_size = other.m_size;
  other.m_size = tmp_size;

  auto tmp_capacity = m_capacity;
  m_capacity = other.m_capacity;
  other.m_capacity = tmp_capacity;
}




