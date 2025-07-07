/*
* litetest.h - A light weight single header unittest utility
*
* Based on https://medium.com/@minchopaskal/write-your-own-c-unit-testing-library-2a9bf19ce2e0
* and updated by Nilay Kothari
*
* DO NOT USE TO TEST PRODUCTION PROJECTS!!
*/
#pragma once
#include <string>
#include <exception>
#include <unordered_map>
#include <source_location>
#include <filesystem>
#include <iostream>
// #include <chrono>

namespace litetest {

  namespace detail {
    using TestFunc = void(*)();

    extern std::unordered_map<std::string, TestFunc> tests;

    struct TestException : std::exception {
      std::string err;
      TestException(std::string err) : err(err) {}
      const char* what() const noexcept override {
        return err.c_str();
      }
    };

    inline void addTest(const std::string& name, TestFunc f) {
      tests.insert({ name, f });
    }
  }

  inline void runTests() {
    for (auto& [name, test] : detail::tests) {
      std::cout << "Running test " << name << std::flush;
      try {
        auto start = std::chrono::high_resolution_clock::now();
        test();
        const std::chrono::duration<double, std::milli> duration
          = std::chrono::high_resolution_clock::now() - start;
        std::cout << "\r[PASS]  " << name << " (" << duration.count() << "ms)" << std::flush;
      }
      catch (const detail::TestException& e) {
        std::cout << "\r[FAIL]* " << name << " at " << e.what() << std::flush;
      }
      std::cout << "\n";
    }
  }
}

#define INITIALIZE_LTEST(...) \
namespace litetest::detail{ \
std::unordered_map<std::string, TestFunc> tests{}; \
}



#define LTEST_ASSERT(expr) \
do { \
  auto loc = std::source_location::current(); \
  if (!(expr)) { \
    auto p = std::filesystem::path(std::string(loc.file_name())); \
    throw litetest::detail::TestException(p.filename().string() + ":" + std::to_string(loc.line())); \
  } \
} while (false)

#define LTEST_FUNCTION(category, name) \
void test_ ##category ## _ ## name(); \
namespace litetest { \
  namespace detail { \
    struct Register_ ## category ## _ ## name { \
      Register_ ## category ## _ ## name() { \
        addTest(#category "::" #name, &test_ ## category ## _ ## name); \
      } \
    } category ## _ ## name ## _Registrator; \
  } \
}\
void test_ ## category ## _ ## name()
