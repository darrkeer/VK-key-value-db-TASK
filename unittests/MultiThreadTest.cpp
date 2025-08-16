#include <gtest/gtest.h>

#include <atomic>
#include <vector>
#include <string>
#include <thread>
#include <functional>

#include "../src/KVStorage.h"

using namespace detail;

void ParallelTest(const std::function<bool(std::size_t)>& f, std::size_t threads_cnt=2) {
    std::vector<std::thread> threads;
    std::vector<std::atomic<bool>> check(threads_cnt);
    for(std::size_t t=0; t<threads_cnt; ++t) {
        threads.emplace_back([&check, t, &f](){
            check[t] = f(t);
        });
    }
    for(std::size_t t=0; t<threads_cnt; ++t) {
        threads[t].join();
    }
    for(std::size_t t=0; t<threads_cnt; ++t) {
        ASSERT_TRUE(check[t]);
    }
}

TEST(MultiThread, Read) {
    KVStorage storage;
    storage.set("a", "2", 0);
    ParallelTest(
        [&storage](std::size_t){
            return storage.get("a") == "2"; 
        }, 5
    );
}

TEST(MultiThread, Write) {
    KVStorage storage;
    const std::vector<std::string> data = {"a", "b", "c", "d", "e"};
    ParallelTest(
        [&storage, &data](std::size_t t) {
            storage.set(data[t], "1", 0);
            return true;
        }, 5
    );
    std::vector<std::pair<std::string, std::string>> expected = {
        {"a", "1"}, {"b", "1"}, {"c", "1"}, {"d", "1"}, {"e", "1"}
    };
    ASSERT_EQ(storage.size(), 5);
    ASSERT_EQ(storage.getManySorted("a", 5), expected);
}