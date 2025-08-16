#include <gtest/gtest.h>

#include "../src/Treap.h"

using namespace detail;

void CheckIsSame(const std::vector<std::pair<std::string, std::string>> &values,
                 Treap &t) {
  ASSERT_EQ(values.size(), t.size());
  auto ptr = t.begin();
  std::size_t i = 0;
  while (ptr != t.end()) {
    ASSERT_EQ(key(ptr), values[i].first);
    ASSERT_EQ(value(ptr), values[i].second);
    ++i;
    ptr = next(ptr);
  }
}

TEST(TreapTest, Base) {
  // std::cerr << "start\n";
  Treap t;
  // std::cerr << "created\n";
  t.set("key", "value", 1);
  // std::cerr << "Set\n";
  ASSERT_EQ(t.find("x"), nullptr);
  // std::cerr << "find\n";
  ASSERT_EQ(value(t.find("key")), "value");
  // std::cerr << "done\n";
  ASSERT_EQ(t.size(), 1);
}

TEST(TreapTest, Advanced) {
  Treap t;
  t.set("c", "3", 0);
  t.set("a", "1", 0);
  t.set("b", "2", 0);
  CheckIsSame({{"a", "1"}, {"b", "2"}, {"c", "3"}}, t);
}

TEST(TreapTest, Remove) {
  Treap t;
  t.set("x", "y", 0);
  t.set("z", "y", 0);
  CheckIsSame({{"x", "y"}, {"z", "y"}}, t);
  ASSERT_TRUE(t.remove("x"));
  ASSERT_FALSE(t.remove("w"));
  CheckIsSame({{"z", "y"}}, t);
  ASSERT_TRUE(t.remove("z"));
}

TEST(TreapTest, GetManySorted) {
  Treap t;
  t.set("c", "3", 0);
  t.set("a", "1", 0);
  t.set("b", "2", 0);
  std::vector<std::pair<std::string, std::string>> expected = {
      {"a", "1"}, {"b", "2"}, {"c", "3"}};
  ASSERT_EQ(t.getManySorted("a", 3), expected);
  expected = {{"a", "1"}, {"b", "2"}};
  ASSERT_EQ(t.getManySorted("a", 2), expected);
  expected = {{"b", "2"}, {"c", "3"}};
  ASSERT_EQ(t.getManySorted("b", 2), expected);
  expected = {};
  ASSERT_EQ(t.getManySorted("a", 0), expected);
}