#include "litetest.h"
#include "src/open_addressing_hash_map.h"

LTEST_FUNCTION(OpenAddressingTests, initial_state) {
  OpenAddressingHashMap<uint64_t, uint64_t> map;
  LTEST_ASSERT(map.size() == 0);
  LTEST_ASSERT(map.empty());
  LTEST_ASSERT(map.capacity() == 25013);
  LTEST_ASSERT(map.get(1234).has_value() == false);
}

LTEST_FUNCTION(OpenAddressingTests, insert_and_get_single_value) {
  OpenAddressingHashMap<uint64_t, uint64_t> map;

  map.insert(50000, 100);

  LTEST_ASSERT(map.size() == 1);
  LTEST_ASSERT(!map.empty());
  auto v = map.get(50000);
  LTEST_ASSERT(v.has_value() &&  *v.value() == 100);
}

LTEST_FUNCTION(OpenAddressingTests, insert_multiple_values) {
  OpenAddressingHashMap<uint64_t, uint64_t> map;

  map.insert(10000, 100);
  map.insert(20000, 200);
  map.insert(30000, 300);
  map.insert(40000, 400);

  LTEST_ASSERT(map.size() == 4);
  auto v = map.get(10000);
  LTEST_ASSERT(v.has_value() && *v.value() == 100);
  v = map.get(20000);
  LTEST_ASSERT(v.has_value() && *v.value() == 200);
  v = map.get(30000);
  LTEST_ASSERT(v.has_value() && *v.value() == 300);
  v = map.get(40000);
  LTEST_ASSERT(v.has_value() && *v.value() == 400);
}

LTEST_FUNCTION(OpenAddressingTests, update_values) {
  OpenAddressingHashMap<uint64_t, uint64_t> map;
  map.insert(10000, 100);
  map.insert(20000, 200);
  map.insert(30000, 300);
  map.insert(40000, 400);

  map.insert(20000, 2001);
  map.insert(30000, 3001);

  LTEST_ASSERT(map.size() == 4);
  auto v = map.get(10000);
  LTEST_ASSERT(v.has_value() && *v.value() == 100);
  v = map.get(20000);
  LTEST_ASSERT(v.has_value() && *v.value() == 2001);
  v = map.get(30000);
  LTEST_ASSERT(v.has_value() && *v.value() == 3001);
  v = map.get(40000);
  LTEST_ASSERT(v.has_value() && *v.value() == 400);
}

LTEST_FUNCTION(OpenAddressingTests, hash_collisions) {
  struct BadHasher{
    std::size_t operator()(uint64_t const& k) {
      return 2;
    } 
  };

  OpenAddressingHashMap<uint64_t, uint64_t, 13, BadHasher> map;
  map.insert(10000, 100);
  map.insert(20000, 200);
  map.insert(30000, 300);
  map.insert(40000, 400);
  map.insert(50000, 500);

  map.insert(20000, 2001);
  map.insert(30000, 3001);

  LTEST_ASSERT(map.size() == 5);
  auto v = map.get(10000);
  LTEST_ASSERT(v.has_value() && *v.value() == 100);
  v = map.get(20000);
  LTEST_ASSERT(v.has_value() && *v.value() == 2001);
  v = map.get(30000);
  LTEST_ASSERT(v.has_value() && *v.value() == 3001);
  v = map.get(40000);
  LTEST_ASSERT(v.has_value() && *v.value() == 400);
}

