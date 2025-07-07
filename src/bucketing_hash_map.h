#pragma once 

#include <utility>
#include <optional>
#include <vector>
#include <memory>
#include <cmath>
#include <variant>
#include <functional>


template<typename K, typename V, const size_t C=54917, typename Hasher = std::hash<K>>
class BucketingHashMap {
public:
  static constexpr size_t NUM_BUCKETS = C;  // Use a prime number for better distribution and to avoid collisions
                                         // ASSUMPTION!!: The stored dataset will not exceed this NUM_BUCKETS
  using Key = K;
  using Value = V;

  // inserts a new key-value pair or replaces a key's existing value
  void insert(Key const key, Value const value) {
    auto node = _find_node(key);
    if (node->get() != nullptr) {
      (*node)->value = value;
    } else {
      *node = std::make_unique<Node>(key, value);
      ++_size;
    }
  };

  // returns pointer to the value of the corresponding key (if exists)
  std::optional<Value const*> get(Key const& key) {
    auto node = _find_node(key);
    if (node->get()) {
        return std::optional<Value const*>(&(*node)->value); 
    }
    return std::nullopt;
  }

  // returns the number of key-value pairs in the map
  size_t size() const { return _size; }

  // returns true if the map is empty, false otherwise
  bool empty() const { return _size == 0; }

  // returns the maximum number of key-value pairs that the map can hold
  size_t num_buckets() const { return NUM_BUCKETS; }

private:
  struct Node {
    Key key;
    Value value;
    std::unique_ptr<Node> next;
    Node(const Key& k, const Value& v) : key(k), value(v) {}
  };

  std::vector<std::unique_ptr<Node>> _table{ NUM_BUCKETS };
  size_t _size = 0;

  std::unique_ptr<Node>* _find_node(Key const& key) {
    size_t bucket_index =  Hasher{}(key) % NUM_BUCKETS;
    
    std::unique_ptr<Node>* p_node = &_table[bucket_index];
    while (p_node->get() != nullptr && (*p_node)->key != key) {
      p_node = std::addressof((*p_node)->next);
    }
    return std::addressof(*p_node);
  }
};
