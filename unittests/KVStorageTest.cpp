#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <vector>

#include "../src/KVStorage.h"

using namespace detail;

TEST(KVStorage, Base) {
  KVStorage storage;
  storage.set("a", "1", 0);
  storage.set("b", "2", 0);
  std::vector<std::pair<std::string, std::string>> expected = {{"a", "1"},
                                                               {"b", "2"}};
  EXPECT_EQ(storage.getManySorted("a", 2), expected);
  storage.removeOneExpiredEntry();
  storage.set("c", "3", 0);
  expected = {{"a", "1"}, {"b", "2"}, {"c", "3"}};
  EXPECT_EQ(storage.getManySorted("a", 3), expected);
}

TEST(KVStorage, TTL) {
  KVStorage storage;
  storage.set("a", "1", 2);
  auto respond = storage.get("a");
  ASSERT_TRUE(respond.has_value());
  ASSERT_EQ(*respond, "1");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  respond = storage.get("a");
  ASSERT_FALSE(respond.has_value());
}

TEST(KVStorage, TTL_advanced) {
  KVStorage storage;
  storage.set("a", "1", 0);
  auto respond = storage.get("a");
  ASSERT_TRUE(respond.has_value());
  ASSERT_EQ(*respond, "1");

  storage.set("a", "1", 1);
  respond = storage.get("a");
  ASSERT_TRUE(respond.has_value());
  ASSERT_EQ(*respond, "1");

  std::this_thread::sleep_for(std::chrono::seconds(2));
  respond = storage.get("a");
  ASSERT_FALSE(respond.has_value());
}