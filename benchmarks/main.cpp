#include <iostream>
#include <numeric>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <limits>
#include <chrono>
#include "src/bucketing_hash_map.h"
#include "src/open_addressing_hash_map.h"

void random_fill_vector(std::vector<uint64_t>& vec) {
    const size_t vec_size = vec.size();
    vec.clear();

    std::random_device rd; 
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> distrib(0, std::numeric_limits<uint64_t>::max());
 
    std::unordered_set<uint64_t> unique_numbers;
    while(unique_numbers.size() != vec_size) {
        auto i = distrib(gen);
        if (unique_numbers.insert(i).second == true) {
            vec.push_back(i);
        }
    }
}

#define BENCHMARK(msg, f) \
{ \
   auto start = std::chrono::high_resolution_clock::now(); \
    (f); \
    const std::chrono::duration<double, std::milli> duration \
        = std::chrono::high_resolution_clock::now() - start; \
    std::cout << msg << duration.count(); \
}\

template<typename Map>
void insert_elements(Map& map, std::vector<uint64_t>& values, size_t count) {
    std::for_each_n(
        values.begin(),
        count,
        [&map](auto n) {
            map.insert(n, n);
        } 
    );
};


template<
    const size_t oa_capacity = 54917,
    const size_t bucket_count = 54917
>
void benchmark_insertions(std::vector<uint64_t>& data, const size_t data_count)
{
    OpenAddressingHashMap<uint64_t, uint64_t, oa_capacity> open_addressing_map;
    BucketingHashMap<uint64_t, uint64_t, bucket_count> bucketting_map;

    std::cout << "  {" << std::endl;
    std::cout << "    \"data_count\": " << data_count << "," << std::endl;
    std::cout << "    \"bucket_addressing_load_factor\": "
        << (data_count * 100.0) / bucketting_map.num_buckets() << ", " <<  std::endl;
    std::cout << "    \"open_addressing_load_factor\": "
        << (data_count * 100.0) / open_addressing_map.capacity() << ", " << std::endl;

    BENCHMARK(
        "    \"bucketting_addressing_time\": ",
        insert_elements(bucketting_map, data, data_count)
    );
    std::cout << "," << std:: endl;
    BENCHMARK(
        "    \"open_addressing_time\": ",
        insert_elements(open_addressing_map, data, data_count)
    );
    std::cout << "  }," << std::endl;
}

template<typename Map>
void look_up_elements(Map& map, std::vector<uint64_t>& values, size_t count) {
    std::for_each_n(
        values.begin(),
        count,
        [&map](auto n) {
            auto v = map.get(n);
            if (!(v.has_value() && *v.value() == n)) {
                throw std::runtime_error("PANIC! Incorrect item !!");
            }
        } 
    );
};

template<
    const size_t oa_capacity = 54917,
    const size_t bucket_count = 54917
>
void benchmark_lookups(
    std::vector<uint64_t>& data,
    const size_t data_count
)
{
    OpenAddressingHashMap<uint64_t, uint64_t, oa_capacity> open_addressing_map;
    insert_elements(open_addressing_map, data, data_count);

    BucketingHashMap<uint64_t, uint64_t, bucket_count> bucketting_map;
    insert_elements(bucketting_map, data, data_count);

    std::cout << "  {" << std::endl;
    std::cout << "    \"data_count\": " << data_count << "," << std::endl;
    std::cout << "    \"bucket_addressing_load_factor\": "
        << (data_count * 100.0) / bucketting_map.num_buckets() << ", " << std::endl;
    std::cout << "    \"open_addressing_load_factor\": "
        << (data_count * 100.0) / open_addressing_map.capacity() << ", " << std::endl;

    BENCHMARK(
        "    \"bucketting_addressing_time\": ",
        look_up_elements(open_addressing_map, data, data_count)
    );
    std::cout << "," << std:: endl;
    BENCHMARK(
        "    \"open_addressing_time\": ",
        look_up_elements(open_addressing_map, data, data_count)
    );
    std::cout << "  }," << std::endl;
}


int main() {
    std::cout << "Hello benches" << std::endl;

    std::vector<uint64_t> data(1'00'000);
    random_fill_vector(data);
    std::cout << "[" << std::endl;

    std::cout << "{\n  \"bench_category\": \"Insertion with increasing load factors\"," << std::endl;
    std::cout << "  \"results\": [" << std::endl;
    benchmark_insertions(data, 10000);
    benchmark_insertions(data, 20000);
    benchmark_insertions(data, 25000);
    benchmark_insertions(data, 30000);
    benchmark_insertions(data, 40000);
    benchmark_insertions(data, 50000);
    benchmark_insertions(data, 54916);
    std::cout << "  ]\n}," << std::endl;

    std::cout << "{\n  \"bench_category\": \"Look-up tests with increasing load factors\", " << std::endl;
    std::cout << "  \"results\": [" << std::endl;
    benchmark_lookups(data, 10000);
    benchmark_lookups(data, 20000);
    benchmark_lookups(data, 25000);
    benchmark_lookups(data, 30000);
    benchmark_lookups(data, 40000);
    benchmark_lookups(data, 50000);
    benchmark_lookups(data, 54916);
    std::cout << "  ]\n}," << std::endl;

    std::cout << "{\n  \"bench_category\": \"Insertion tests low bucket counts and increasing load factor\"," << std::endl;
    std::cout << "  \"results\": [" << std::endl;  
    benchmark_insertions<54917, 503>(data, 25000);
    benchmark_insertions<54917, 503>(data, 30000);
    benchmark_insertions<54917, 503>(data, 40000);
    benchmark_insertions<54917, 503>(data, 50000);
    benchmark_insertions<54917, 503>(data, 54916);
    std::cout << "  ]\n}," << std::endl;

    std::cout << "{\n  \"bench_category\": \"Lookup tests low bucket counts and increasing load factor\"," << std::endl;
    std::cout << "  \"results\": [" << std::endl; 
    benchmark_lookups<54917, 503>(data, 25000);
    benchmark_lookups<54917, 503>(data, 30000);
    benchmark_lookups<54917, 503>(data, 40000);
    benchmark_lookups<54917, 503>(data, 50000);
    benchmark_lookups<54917, 503>(data, 54916);
    std::cout << "  ]\n}," << std::endl;

    std::cout << "]" << std::endl;
}