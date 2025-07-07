#include "litetest.h"
#include "src/bucketing_hash_map.h"

LTEST_FUNCTION(BucketingTests, test_initial_state) {
  BucketingHashMap<uint64_t, uint64_t> map;
  LTEST_ASSERT(map.size() == 0);
  LTEST_ASSERT(map.empty());
  LTEST_ASSERT(map.get(1234).has_value() == false);
}

LTEST_FUNCTION(BucketingTests, test_insert_and_get_single_value) {
  BucketingHashMap<uint64_t, uint64_t> map;

  map.insert(50000, 100);

  LTEST_ASSERT(map.size() == 1);
  LTEST_ASSERT(!map.empty());
  auto v = map.get(50000);
  LTEST_ASSERT(v.has_value() &&  *v.value() == 100);
}

LTEST_FUNCTION(BucketingTests, test_insert_multiple_values) {
  BucketingHashMap<uint64_t, uint64_t> map;

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

LTEST_FUNCTION(BucketingTests, test_update_values) {
  BucketingHashMap<uint64_t, uint64_t> map;
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


LTEST_FUNCTION(BucketingTests, fewer_buckets_than_items) {
  BucketingHashMap<uint64_t, uint64_t, 2, std::hash<uint64_t>> map;
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
  v = map.get(50000);
  LTEST_ASSERT(v.has_value() && *v.value() == 500);
}
