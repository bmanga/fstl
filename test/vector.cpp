#include <catch2/catch.hpp>
#include <stdexcept>
#include <set>

#define TEST_STD_VEC 0
#if TEST_STD_VEC
#include <vector>
template <class T>
using vector = std::vector<T>;
#else
#include "fstl/vector.h"
template <class T>
using vector = fstl::vector<T>;
#endif

TEST_CASE("vector::empty", "[ctor]") {
  vector<int> vi;
  CHECK(vi.size() == 0);
  REQUIRE(vi.empty());
  vi.push_back(1);
  REQUIRE(!vi.empty());
}

TEST_CASE("vector::default_sized", "[ctor]") {
  vector<int> vi(100);
  REQUIRE(vi.size() == 100);
  REQUIRE(vi[0] == 0);
  REQUIRE(vi[99] == 0);
}

TEST_CASE("vector::inited_size", "[ctor]") {
  vector<int> vi(100, 10);
  REQUIRE(vi.size() == 100);
  REQUIRE(vi[0] == 10);
  REQUIRE(vi[99] == 10);
}

TEST_CASE("vector::vector(initializer_list)", "[ctor]") {
  vector<int> vi{0, 1, 2};
  REQUIRE(vi.size() == 3);
  REQUIRE(vi[0] == 0);
  REQUIRE(vi[1] == 1);
  REQUIRE(vi[2] == 2);
}

TEST_CASE("vector::copy", "[ctor]") {
  vector<int> orig(100, 10);
  vector<int> copy = orig;
  REQUIRE(orig.size() == 100);
  REQUIRE(orig[0] == 10);
  REQUIRE(orig[99] == 10);

  REQUIRE(copy.size() == 100);
  REQUIRE(copy[0] == 10);
  REQUIRE(copy[99] == 10);
}

TEST_CASE("vector::move", "[ctor]") {
  vector<int> orig(100, 10);
  vector<int> moved = std::move(orig);
  REQUIRE(orig.size() == 0);
  REQUIRE(orig.capacity() == 0);

  REQUIRE(moved.size() == 100);
  REQUIRE(moved[0] == 10);
  REQUIRE(moved[99] == 10);
}

TEST_CASE("vector::vector(It, It)", "[ctor]") {
  vector<int> vi1;
  vi1.push_back(0);
  vi1.push_back(1);

  vector<int> vi(vi1.begin(), vi1.end());
  REQUIRE(vi.size() == 2);
  REQUIRE(vi[0] == 0);
  REQUIRE(vi[1] == 1);

  std::set<int> si{2, 3};
  vector<int>vi2(si.begin(), si.end());
}

TEST_CASE("vector::at", "[elem_access]") {
  vector<int> vi(100, 10);

  REQUIRE(vi.at(0) == 10);
  vi.at(0) = 20;
  REQUIRE(vi.at(0) == 20);

  REQUIRE_THROWS_AS(vi.at(100), std::out_of_range);
}

TEST_CASE("vector::operator[]", "[elem_access]") {
  vector<int> vi(100, 10);

  REQUIRE(vi[0] == 10);
  vi[0] = 20;
  REQUIRE(vi[0] == 20);
}

TEST_CASE("vector::front_back", "[elem_access]") {
  vector<int> vi;
  vi.push_back(10);
  vi.push_back(30);
  REQUIRE(vi.front() == 10);
  REQUIRE(vi.back() == 30);
  vi.front() = 1;
  vi.back() = 2;
  REQUIRE(vi.front() == 1);
  REQUIRE(vi.back() == 2);
}

TEST_CASE("vector::data", "[elem_access]") {
  vector<int> vi;
  REQUIRE(vi.data() == nullptr);
  vi.push_back(1);
  REQUIRE(*vi.data() == 1);
}

TEST_CASE("vector::clear", "[modifiers]") {
  vector<int> vi(100);
  CHECK(vi.size() == 100);
  CHECK(vi.capacity() == 100);
  vi.clear();
  REQUIRE(vi.size() == 0);
  REQUIRE(vi.capacity() == 100);
}

TEST_CASE("vector::insert", "[modifiers]") {
  vector<int> vi(100);
  CHECK(vi.size() == 100);
  CHECK(vi.capacity() == 100);
  auto it = vi.insert(vi.begin(), 10);
  REQUIRE(*it == 10);
  it = vi.insert(vi.end(), 20);
  REQUIRE(*it == 20);
  it = vi.insert(vi.begin() + 50, 30);
  REQUIRE(*it == 30);
  REQUIRE(vi.size() == 103);
  REQUIRE(vi[0] == 10);
  REQUIRE(vi[50] == 30);
  REQUIRE(vi[102] == 20);

  vi.clear();
  vi.push_back(0);
  vi.push_back(2);
  vi.insert(vi.begin() + 1, 1);
  REQUIRE(vi.size() == 3);
  REQUIRE(vi[0] == 0);
  REQUIRE(vi[1] == 1);
  REQUIRE(vi[2] == 2);

  // Ranged insert
  vi.clear();
  vi.push_back(1);
  vi.push_back(4);

  vector<int> vi2;
  vi2.push_back(2);
  vi2.push_back(3);
  CHECK(vi2.size() == 2);
  CHECK(*vi2.begin() == 2);
  CHECK(*(vi2.begin() + 1) == 3);

  vi.insert(vi.begin() + 1, vi2.begin(), vi2.end());
  CHECK(vi2.size() == 2);
  REQUIRE(vi.size() == 4);
  REQUIRE(vi[0] == 1);
  REQUIRE(vi[1] == 2);
  REQUIRE(vi[2] == 3);
  REQUIRE(vi[3] == 4);
}

TEST_CASE("vector::emplace", "[modifiers]") {
  struct dummy {
    int a = 0; float b = 0;
    dummy() = default;
    dummy(int a, float b) : a(a), b(b) {}
  };
  vector<dummy> vd(100);
  CHECK(vd.size() == 100);
  CHECK(vd.capacity() == 100);
  vd.emplace(vd.begin(), 1, 2.f);
  vd.emplace(vd.end(), 10, 20.f);
  vd.emplace(vd.begin() + 50, 100, 300.f);
  REQUIRE(vd.size() == 103);
  REQUIRE(vd[0].b == 2.f);
  REQUIRE(vd[50].a == 100);
  REQUIRE(vd[102].b == 20.f);
}

TEST_CASE("vector::erase", "[modifiers]") {
  vector<int> vi;
  vi.push_back(1);
  vi.push_back(2);
  vi.push_back(3);
  CHECK(vi.size() == 3);
  auto it = vi.erase(vi.begin());
  REQUIRE(vi.size() == 2);
  REQUIRE(*it == 2);
  REQUIRE(vi[0] == 2);
  REQUIRE(vi[1] == 3);

  vi.erase(vi.begin(), vi.end());
  REQUIRE(vi.size() == 0);
  vi.push_back(1);
  vi.push_back(2);
  vi.push_back(3);
  vi.push_back(4);
  it = vi.erase(vi.begin() + 1, vi.begin() +3);
  REQUIRE(vi.size() == 2);
  REQUIRE(*it == 4);
  REQUIRE(vi[0] == 1);
  REQUIRE(vi[1] == 4);
}

static int copy = 0, move = 0;
TEST_CASE("vector::push_back", "[modifiers]") {
  copy = move = 0;
  struct dummy
  {
    dummy() = default;
    explicit dummy(int) {}
    dummy(const dummy &) { ++copy; }
    dummy(dummy &&) noexcept { ++move; }
  };
  vector<dummy> v;
  v.reserve(4);
  v.push_back(dummy{});
  v.push_back(dummy{1});
  dummy d{};
  v.push_back(d);
  v.push_back(d);
  REQUIRE(v.size() == 4);
  REQUIRE(copy == 2);
  REQUIRE(move == 2);

  // Ensure resizing moves
  v.push_back(d);
  REQUIRE(move == 2 + 4);
}

TEST_CASE("vector::emplace_back", "[modifiers]") {
  copy = move = 0;
  struct dummy
  {
    dummy() = default;
    explicit dummy(int) {}
    dummy(const dummy &) { ++copy; }
    dummy(dummy &&) noexcept { ++move; }
  };
  vector<dummy> v;
  v.reserve(2);
  v.emplace_back();
  v.emplace_back(1);
  REQUIRE(v.size() == 2);
  REQUIRE(copy == 0);

  // behavior is slightly different here, we can't emplace in fstl.
#if TEST_STD_VEC
  REQUIRE(move == 0);
  v.emplace_back(2);
  REQUIRE(move == 2);
#else
  REQUIRE(move == 2);
  v.emplace_back(2);
  REQUIRE(move == 2 + 2 + 1);
#endif
}

TEST_CASE("vector::pop_back", "[modifiers]") {
  vector<int> vi;
  vi.reserve(3);
  vi.push_back(1);
  vi.push_back(2);
  vi.push_back(3);
  CHECK(vi.size() == 3);
  CHECK(vi.capacity() == 3);
  vi.pop_back();
  REQUIRE(vi.size() == 2);
  REQUIRE(vi.capacity() == 3);
  REQUIRE(vi.back() == 2);
}

TEST_CASE("vector::resize", "[modifiers]") {
  copy = move = 0;
  struct dummy
  {
    dummy() = default;
    explicit dummy(int) {}
    dummy(const dummy &) { ++copy; }
    dummy(dummy &&) noexcept { ++move; }
  };

  vector<dummy> v;
  v.resize(5);

  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() == 5);
  REQUIRE(move == 0);

  v.resize(10);
  REQUIRE(v.size() == 10);
  REQUIRE(v.capacity() == 10);
  REQUIRE(move == 5);

  move = 0;
  v.resize(5);
  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() == 10); // Capacity does not shrink
  REQUIRE(move == 0); // No moves done

  // Resize with default value
  vector<int> vi;
  vi.resize(2, 10);
  REQUIRE(vi[0] == 10);
  REQUIRE(vi[1] == 10);
}

TEST_CASE("vector::swap", "[modifiers]") {
  copy = move = 0;
  struct dummy {
    int x = 0;
    dummy() = default;
    explicit dummy(int) {}
    dummy(const dummy &) { ++copy; }
    dummy(dummy &&) noexcept { ++move; }
  };

  vector<dummy> v;
  v.resize(5);
  v[0].x = 5;
  vector<dummy> v2;
  v2.resize(10);
  v2[0].x = 10;

  v.swap(v2);
  REQUIRE(v.size() == 10);
  REQUIRE(v.capacity() == 10);
  REQUIRE(v[0].x == 10);
  REQUIRE(v2.size() == 5);
  REQUIRE(v2.capacity() == 5);
  REQUIRE(v[0].x == 10);
  REQUIRE(move == 0);
  REQUIRE(copy == 0);
}

TEST_CASE("vector::iterator", "[iterators]") {
  vector<int> vi;
  vi.push_back(1);
  vi.push_back(2);

  int sum = 0;
  for (const int &i : vi) {
    sum += i;
  }

  const vector<int>& cvi = vi;
  for (const int &i : cvi) {
    sum += i;
  }

  REQUIRE(sum == 6);
}

TEST_CASE("vector::reverse_iterator", "[iterators]") {
  vector<int> vi;
  vi.push_back(1);
  vi.push_back(2);
  vi.push_back(3);

  int sum = 0;
  int multiply = 1;

  for (auto it = vi.rbegin(); it != vi.rend(); ++it) {
    sum  = sum * 10 + *it;
  }

  REQUIRE(sum == 321);
  sum = 0;
  const vector<int>& cvi = vi;
  for (auto it = cvi.rbegin(); it != cvi.rend(); ++it) {
    sum  = sum * 10 + *it;
  }
  REQUIRE(sum == 321);

  // Conversion
  const vector<int>::const_reverse_iterator ri = vi.rbegin();
}

TEST_CASE("no_default_ctor", "[types]") {
  struct dummy
  {
    dummy() = delete;
    dummy(int) {};
  };

  vector<dummy> vd;
}