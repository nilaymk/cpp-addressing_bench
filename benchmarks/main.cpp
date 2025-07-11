#include <iostream>
#include <numeric>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <limits>
#include <chrono>
#include <format>
#include <tuple>
#include <utility>

#include <iostream>
#include <tuple>
#include <utility>

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


template<typename Map>
void insert_elements(Map& map, std::vector<uint64_t>& keys, size_t count) {
    for (auto i = 0; i < count; ++i) {
        map.insert(keys[i], keys[i]);
    } 
};

template<typename Map>
void look_up_elements(Map& map, std::vector<uint64_t>& keys, size_t count) {
    for (auto i = 0; i < count; ++i) {
        auto v = map.get(keys[i]);
        if (!(v.has_value() && *v.value() == keys[i])) {
            throw std::runtime_error("PANIC! Incorrect item !!");
        } 
    };
};
struct Benchmarks {
    void benchmark(
        std::string const& category,
        std::string const& name,
        std::string const& description,
        std::function<void ()> benchmarked_fn
     ) {
        auto start = std::chrono::high_resolution_clock::now();
        benchmarked_fn();
        const std::chrono::duration<double, std::milli> duration =
            std::chrono::high_resolution_clock::now() - start;
        this->results.emplace_back(BenchmarkItem{
            category,
            name,
            description,
            duration
        });
     }

    struct BenchmarkItem {
        std::string category;
        std::string name;
        std::string description;
        const std::chrono::duration<double, std::milli> duration;
    };

    std::string name;
    std::vector<BenchmarkItem> results;
    Benchmarks(std::string const& name_) : name(name_) {}
};


template<typename T, typename V, size_t... I>
void tuple_visit_impl(T&& t, V&& v, std::index_sequence<I...>)
{
    (..., v(std::get<I>(t)));
}

template<typename T, typename V>
void tuple_visit(T&& t, V&& v)
{
    tuple_visit_impl(std::forward<T>(t), std::forward<V>(v),
        std::make_index_sequence<std::tuple_size<
            typename std::decay<T>::type>::value>());
}

int main() {
    std::vector<uint64_t> input_data(1'00'000);
    random_fill_vector(input_data);

    Benchmarks benchmarks{"Hash Map Addressing Scheme Benchmarks"};

    std::cout << "Category, Name, DataSize, Capacity, LoadFactor, Duration(ms)" << std::endl;

    for (auto data_size : {10000, 20000, 25000, 30000, 40000, 50000, 54916}) {
        {
            OpenAddressingHashMap<uint64_t, uint64_t, 54917> open_addressing_map;
            benchmarks.benchmark(
                "Insertion",
                std::format("Open Addressing ({})", open_addressing_map.capacity()),
                std::format(
                    "{}, {}, {}",
                    data_size,
                    open_addressing_map.capacity(),
                    (double)data_size*100.0/open_addressing_map.capacity()
                ),
                [&] {
                    insert_elements(open_addressing_map, input_data, data_size);
                }
            );

            benchmarks.benchmark(
                "Look Up",
                std::format("Open Addressing ({})", open_addressing_map.capacity()),
                std::format(
                    "{}, {}, {}",
                    data_size,
                    open_addressing_map.capacity(),
                    (double)data_size*100.0/open_addressing_map.capacity()
                ),
                [&] {
                    look_up_elements(open_addressing_map, input_data, data_size);
                }
            );
        }

        {
            auto varying_bucket_sized_maps = std::tuple {
                BucketingHashMap<uint64_t, uint64_t, 503>{},
                BucketingHashMap<uint64_t, uint64_t, 54917>{},
            };

            tuple_visit(
                varying_bucket_sized_maps,
                [&](auto& bucket_map){
                    benchmarks.benchmark(
                        "Insertion",
                        std::format("Bucket Addressing ({})", bucket_map.num_buckets()),
                        std::format(
                            "{}, {}, {}",
                            data_size,
                            bucket_map.num_buckets(),
                            (double)data_size*100.0/bucket_map.num_buckets()
                        ),
                        [&] {
                            insert_elements(bucket_map, input_data, data_size);
                        }
                    );
                }
            );

            tuple_visit(
                varying_bucket_sized_maps,
                [&](auto& bucket_map){
                    benchmarks.benchmark(
                        "Look Up",
                        std::format("Bucket Addressing ({})", bucket_map.num_buckets()),
                        std::format(
                            "{}, {}, {}",
                            data_size,
                            bucket_map.num_buckets(),
                            (double)data_size*100.0/bucket_map.num_buckets()
                        ),
                        [&] {
                            look_up_elements(bucket_map, input_data, data_size);
                        }
                    );
                }
            );
        }
    }

    for (auto&& benchmark_item: benchmarks.results) {
        std::cout
            << benchmark_item.category << ", "
            << benchmark_item.name << ", "
            << benchmark_item.description << ", "
            << benchmark_item.duration.count() << std::endl;
    }
}