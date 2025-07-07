#pragma once 

#include <utility>
#include <optional>
#include <vector>
#include <memory>
#include <cmath>
#include <variant>
#include <functional>


template<typename K, typename V, const size_t C=54917, typename Hasher = std::hash<K>>
class OpenAddressingHashMap {
public:
  static constexpr size_t CAPACITY = C;  // Use a prime number for better distribution and to avoid collisions
                                         // ASSUMPTION!!: The stored dataset will not exceed this capacity
  using Key = K;
  using Value = V;

  // inserts a new key-value pair or replaces a key's existing value
  void insert(Key const key, Value const value) {
    auto const index = _find_index(key);
    if (_size == CAPACITY || index == CAPACITY) {
      throw std::runtime_error("HashMap is full, cannot insert new key-value pair.");
    }

    if (auto occupied_node = std::get_if<OccupiedNode>(&_table[index])) {
      occupied_node->value = value;
    } else {
      _table[index].template emplace<OccupiedNode>(key, value);
      ++_size;
    }
  };

  // returns pointer to the value of the corresponding key (if exists)
  std::optional<Value const*> get(Key const& key) const {
    auto const index = _find_index(key);

    OccupiedNode const* occupied_node {nullptr};
    if (index != CAPACITY && (occupied_node = std::get_if<OccupiedNode>(&_table[index]))) {
        return std::optional<Value const*>(&occupied_node->value); 
    }
    return std::nullopt;
  }

  // returns the number of key-value pairs in the map
  size_t size() const { return _size; }

  // returns true if the map is empty, false otherwise
  bool empty() const { return _size == 0; }

  // returns the maximum number of key-value pairs that the map can hold
  size_t capacity() const { return CAPACITY; }

private:
  struct OccupiedNode {
    Key key;
    Value value;
    OccupiedNode(const Key& k, const Value& v) : key(k), value(v) {}
  };
  struct EmptyNode {};
  using Node = std::variant<EmptyNode, OccupiedNode>;

  std::vector<Node> _table{ CAPACITY };
  size_t _size = 0;

  size_t _find_index(Key const& key) const {
    size_t already_visited =  Hasher{}(key) % CAPACITY;
    size_t index = already_visited;

    OccupiedNode const* occupied_node {nullptr};
    while ( (occupied_node = std::get_if<OccupiedNode>(&_table[index])) && occupied_node->key != key) {
      index = (index + 1) % CAPACITY; // linear probing
      if (index == already_visited) {
        return CAPACITY;
      }
    }
    return index;
  }
};
