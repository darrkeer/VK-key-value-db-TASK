#include <gtest/gtest.h>

#include <chrono>
#include <vector>
#include <string>

#include "../src/KVStorage.h"

using namespace detail;

TEST(KVStorage, Base) {
    KVStorage storage;
    storage.set("a", "1", 0);
    storage.set("b", "2", 0);
    std::vector<std::pair<std::string, std::string>> expected = {{"a", "1"}, {"b", "2"}};
    EXPECT_EQ(storage.getManySorted("a", 2), expected);
    storage.removeOneExpiredEntry();
    storage.set("c", "3", 0);
    expected = {{"a", "1"}, {"b", "2"}, {"c", "3"}};
    EXPECT_EQ(storage.getManySorted("a", 3), expected);
}
