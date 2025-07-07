cmake_minimum_required (VERSION 3.10)

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


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET addressing_bench_tests PROPERTY CXX_STANDARD 20)
endif()

