#include <catch2/catch.hpp>

#define TEST_STD_FL 0
#if TEST_STD_FL
#include <forward_list>

template <class T>
using forward_list = std::forward_list<T>;
#else

#include "fstl/forward_list.h"
template <class T>
using forward_list = fstl::forward_list<T>;
#endif

template <class T>
int fl_size(forward_list<T> &fwd)
{
  int size = 0;
  for (const auto &_ : fwd) ++size;
  return size;
}

TEST_CASE("forward_list::empty", "[ctor]") {
  forward_list<int> li;
  REQUIRE(li.empty());
  li.push_front(1);
  REQUIRE(!li.empty());
}

TEST_CASE("forward_list::iterator", "[iterators]") {
  forward_list<int> li;
  li.push_front(1);
  li.push_front(2);
  li.push_front(3);
  li.push_front(4);
  int sum = 0;
  for (int i : li) sum += i;
  REQUIRE(sum == 10);
}

static int dflt = 0, copy = 0, move = 0, destroy = 0;


TEST_CASE("forward_list::clear", "[modifiers]") {
  struct dummy
  {
    dummy() { ++dflt; };
    explicit dummy(int) {}
    dummy(const dummy &) { ++copy; }
    dummy(dummy &&) noexcept { ++move; }
    ~dummy() { ++destroy; }
  };
  forward_list<dummy> ld;
  ld.push_front(dummy{1});
  ld.push_front(dummy{2});
  CHECK(fl_size(ld) == 2);
  destroy = 0;
  ld.clear();
  REQUIRE(fl_size(ld) == 0);
  REQUIRE(destroy == 2);
}