#pragma once

#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>

namespace detail {

class number_gen {
public:
  number_gen(std::uniform_int_distribution<uint32_t> distrib);

  uint32_t get() const;

private:
  mutable std::mt19937 _gen;
  mutable std::uniform_int_distribution<uint32_t> _distrib;
};

class Node {
public:
  Node(number_gen &gen);

  Node(number_gen &gen, const std::string &key, const std::string &value,
       uint32_t ttl);

  friend Node *left(Node *n);

  friend Node *right(Node *n);

  friend Node *parent(Node *n);

  friend std::string key(Node *n);

  friend std::string value(Node *n);

  friend uint32_t size(Node *n);

  friend uint32_t prio(Node *n);

  friend uint32_t ttl(Node *n);

  void connect_left(Node *n);

  void connect_right(Node *n);

  friend Node *next(Node *n);

  friend Node *leftmost(Node *n);

  // All keys in A must be lower than any in B
  friend Node *merge(Node *l, Node *r);

  friend std::pair<Node *, Node *> split(Node *n, const std::string &x,
                                         bool is_upper = false) {
    if (!n) {
      return {nullptr, nullptr};
    }
    if (less(x, n) || !less(x, n) && !less(n, x) && !is_upper) {
      auto [l, r] = split(n->_left, x, is_upper);
      n->connect_left(r);
      return {l, n};
    } else {
      auto [l, r] = split(n->_right, x, is_upper);
      n->connect_right(l);
      return {n, r};
    }
  }
  void update();

  friend class Treap;

private:
  Node(std::uint32_t prio);

  friend bool less(Node *a, Node *b);

  friend bool less(Node *a, const std::string &s);

  friend bool less(const std::string &s, Node *a);

  std::string _key = "";
  std::string _value = "";
  Node *_left = nullptr;
  Node *_right = nullptr;
  Node *_parent = nullptr;
  uint32_t _ttl = 0;
  uint32_t _size = 1;
  uint32_t _prio;
};

class Treap {
public:
  Treap();

  ~Treap();

  Node *find(const std::string &k) const;

  std::vector<std::pair<std::string, std::string>>
  getManySorted(std::string_view k, uint32_t count) const;

  bool remove(std::string_view k);

  void set(std::string k, std::string value, uint32_t ttl);

  Node *begin() const;

  Node *end() const;

private:
  friend void del(Node *n);

  mutable number_gen _gen;
  mutable Node *_root;
};

} // namespace detail