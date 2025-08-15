#include "Treap.h"

namespace detail {

number_gen::number_gen(std::uniform_int_distribution<uint32_t> distrib)
    : _distrib(distrib) {}

uint32_t number_gen::get() const { return _distrib(_gen); }

Node::Node(number_gen &gen) : Node(gen.get()) {}

Node::Node(number_gen &gen, const std::string &key, const std::string &value,
           uint32_t ttl)
    : Node(gen) {
  _key = key;
  _value = value;
  _ttl = ttl;
}

Node *left(Node *n) { return n == nullptr ? nullptr : n->_left; }

Node *right(Node *n) { return n == nullptr ? nullptr : n->_right; }

Node *parent(Node *n) { return n == nullptr ? nullptr : n->_parent; }

std::string key(Node *n) { return n == nullptr ? "" : n->_key; }

std::string value(Node *n) { return n == nullptr ? "" : n->_value; }

uint32_t size(Node *n) { return n == nullptr ? 0 : n->_size; }

uint32_t prio(Node *n) { return n == nullptr ? 0 : n->_prio; }

uint32_t ttl(Node *n) { return n == nullptr ? 0 : n->_ttl; }

void Node::connect_left(Node *n) {
  if (n != nullptr) {
    n->_parent = this;
  }
  _left = n;
  update();
}

void Node::connect_right(Node *n) {
  if (n != nullptr) {
    n->_parent = this;
  }
  _right = n;
  update();
}

Node *next(Node *n) {
  if (right(n) == nullptr) {
    while (right(parent(n)) == n) {
      n = parent(n);
    }
    n = parent(n);
    return n;
  }
  n = right(n);
  while (left(n) != nullptr) {
    n = left(n);
  }
  return n;
}

Node *leftmost(Node *n) {
  if (n == nullptr) {
    return nullptr;
  }
  while (n->_left) {
    n = n->_left;
  }
  return n;
}

// All keys in A must be lower than any in B
Node *merge(Node *l, Node *r) {
  if (l == nullptr) {
    return r;
  }
  if (r == nullptr) {
    return l;
  }
  if (prio(l) > prio(r)) {
    l->connect_right(merge(l->_right, r));
    return l;
  } else {
    r->connect_left(merge(l, r->_left));
    return r;
  }
}

void Node::update() { _size = size(left(this)) + size(right(this)); }

Node::Node(std::uint32_t prio) : _prio(prio) {}

bool less(Node *a, Node *b) {
  if (!parent(a)) {
    return false;
  }
  if (!parent(b)) {
    return true;
  }
  return key(a) < key(b);
}

bool less(Node *a, const std::string &s) {
  if (!parent(a)) {
    return false;
  }
  return key(a) < s;
}

bool less(const std::string &s, Node *a) {
  if (!parent(a)) {
    return true;
  }
  return s < key(a);
}

void del(Node *n);

void del(Node *n) {
  if (n == nullptr) {
    return;
  }
  del(right(n));
  del(left(n));
  delete n;
}

Treap::~Treap() { del(_root); }

Node *Treap::find(const std::string &k) const {
  auto [l, r] = split(_root, k);
  auto [r1, r2] = split(r, k, true);
  _root = merge(l, merge(r1, r2));
  return r1;
}

std::vector<std::pair<std::string, std::string>>
Treap::getManySorted(std::string_view k, uint32_t count) const {
  auto ptr = find(std::string(k));
  std::vector<std::pair<std::string, std::string>> res;
  for (std::size_t i = 0; i < count; ++i) {
    res.emplace_back(key(ptr), value(ptr));
    ptr = next(ptr);
  }
  return res;
}

bool Treap::remove(std::string_view k) {
  auto [l, r] = split(_root, std::string(k));
  auto [r1, r2] = split(r, std::string(k), true);
  bool res = r1 != nullptr;
  _root = merge(l, r2);
  del(r1);
  return res;
}

void Treap::set(std::string k, std::string value, uint32_t ttl) {
  auto [l, r] = split(_root, k);
  auto [r1, r2] = split(r, k, true);
  if (r1 == nullptr) {
    _root = merge(l, merge(new Node(_gen, k, std::move(value), ttl), r2));
  } else {
    r1->_value = value;
    _root = merge(l, merge(r1, r2));
  }
}

Node *Treap::begin() const { return leftmost(_root); }

Node *Treap::end() const { return _root; }

Treap::Treap()
    : _gen(std::uniform_int_distribution<uint32_t>()), _root(new Node(-1)) {}

} // namespace detail