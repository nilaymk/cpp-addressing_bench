cmake_minimum_required (VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("addressing_bench" LANGUAGES C CXX)

include_directories(.)

# Add source to this project's executable.
add_executable (addressing_bench_tests
	"test/litetest.h" 
	"test/main.cpp"
	"test/test_open_addressing_hash_map.cpp"
	"test/test_bucketing_hash_map.cpp"
	"src/open_addressing_hash_map.h"
	"src/bucketing_hash_map.h"
 )

 add_executable (addressing_bench_benchmarks
	"benchmarks/main.cpp"
	"src/open_addressing_hash_map.h"
	"src/bucketing_hash_map.h"
 )

