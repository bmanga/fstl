#include <catch2/catch.hpp>
#include <stdexcept>

#define TEST_STD_UM 0
#if TEST_STD_UM
#include <unordered_map>

using std::unordered_map;
#else

#include "fstl/unordered_map.h"
using fstl::unordered_map;
#endif

static int dflt = 0, copy = 0, move = 0, destroy = 0;
struct dummy
{
  dummy() { ++dflt; };
  explicit dummy(int) {}
  dummy(const dummy &) { ++copy; }
  dummy(dummy &&) noexcept { ++move; }
  dummy &operator =(const dummy&) = default;
  ~dummy() { ++destroy; }
};


TEST_CASE("unordered_map::size", "[capacity]") {
  unordered_map<int, int> umii(10);
  REQUIRE(umii.size() == 0);
  umii[1] = 0;
  umii[2] = 0;
  REQUIRE(umii.size() == 2);
}

TEST_CASE("unordered_map::operator[]", "[modifiers]") {
  unordered_map<int, int> umii(2);
  umii[1] = 1;
  REQUIRE(umii[0] == 0);
  REQUIRE(umii[1] == 1);

  // Ensure multiple entries in same bucket work
  umii[2] = 2;
  umii[3] = 3;
  umii[4] = 4;
  umii[5] = 5;
  umii[6] = 6;
  umii[7] = 7;
  umii[1000] = 1000;
  umii[9876543] = 9876543;
  REQUIRE(umii[2] == 2);
  REQUIRE(umii[3] == 3);
  REQUIRE(umii[4] == 4);
  REQUIRE(umii[5] == 5);
  REQUIRE(umii[6] == 6);
  REQUIRE(umii[7] == 7);
  REQUIRE(umii[1000] == 1000);
  REQUIRE(umii[9876543] == 9876543);
}

TEST_CASE("unordered_map::iterator", "[iterators]") {
  unordered_map<int, int> umii(3);
  umii[2] = 3;
  umii[1] = 7;
  umii[3] = 10;
  umii[9] = 2;
  int sum_x = 0, sum_y = 0;
  for (auto[x, y] : umii) {
    sum_x += x;
    sum_y += y;
  }

  REQUIRE(sum_x == 15);
  REQUIRE(sum_y == 22);
}

TEST_CASE("unordered_map::at", "[lookup]") {
  unordered_map<int, int> umii(3);
  umii[0] = 0;
  REQUIRE(umii.at(0) == 0);
  REQUIRE_THROWS_AS(umii.at(100), std::out_of_range);
}

TEST_CASE("unordered_map::find", "[lookup]") {
  unordered_map<int, int> umii(3);
  umii[2] = 3;
  umii[1] = 7;
  umii[3] = 10;
  umii[9] = 2;

  auto it1 = umii.find(2);
  auto it2 = umii.find(5);
  REQUIRE(it1 != umii.end());
  REQUIRE(it1->first == 2);
  REQUIRE(it1->second == 3);
  REQUIRE(it2 == umii.end());

  umii[5] = 5;
  it2 = umii.find(5);
  REQUIRE(it2 != umii.end());
  REQUIRE(it2->first == 5);
  REQUIRE(it2->second == 5);
}


TEST_CASE("unordered_map::bucket_size", "[buckets]") {
  unordered_map<int, int> umii(1);
  umii[0] = 5;
  umii[5] = 10;
  REQUIRE(umii.bucket_size(0) == 2);
}

TEST_CASE("unordered_map::clear", "[modifiers]") {
  unordered_map<int, dummy> umid(2);
  umid[0] = dummy{0};
  umid[1] = dummy{1};
  umid[2] = dummy{2};
  umid[3] = dummy{3};
  CHECK(umid.size() == 4);
  destroy = 0;
  umid.clear();
  REQUIRE(umid.size() == 0);
  REQUIRE(destroy == 4);
}