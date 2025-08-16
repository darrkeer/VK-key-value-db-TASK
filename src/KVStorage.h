#pragma once

#include <cstdint>
#include <mutex>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <vector>
#include <shared_mutex>

#include "Treap.h"

namespace detail {

struct DefaultClock {
  uint64_t now() const {
      return std::chrono::steady_clock::now().time_since_epoch().count();
  }
};

struct KeyNode {
  std::string key;
  uint64_t ttl;
};

struct KeyNodeComparator {
  bool operator()(const KeyNode& a, const KeyNode& b) const {
    if(a.ttl == b.ttl) {
      return a.key < b.key;
    }
    return a.ttl < b.ttl;
  }
};

} // detail

template <typename Clock = detail::DefaultClock> class KVStorage {
public:
  explicit KVStorage(Clock clock = Clock()) : _clock(std::move(clock)) {}

  // Инициализирует хранилище переданным множеством записей. Размер span может
  // быть очень большим. Также принимает абстракцию часов (Clock) для
  // возможности управления временем в тестах.
  explicit KVStorage(
      std::span<std::tuple<std::string /*key*/, std::string /*value*/,
                           uint32_t /*ttl*/>>
          entries,
      Clock clock = Clock()) : KVStorage(std::move(clock)) {
    for (const auto &[k, v, ttl] : entries) {
      set(k, v, ttl);
    }
  }

  ~KVStorage() = default;

  // Присваивает по ключу key значение value.
  // Если ttl == 0, то время жизни записи - бесконечность, иначе запись должна
  // перестать быть доступной через ttl секунд. Безусловно обновляет ttl записи.
  void set(std::string key, std::string value, uint32_t ttl) {
    clear_by_ttl();

    std::unique_lock lock(mutex);

    uint64_t end_time = get_end_time(ttl);
    _data.set(key, value, end_time);
    // i wont delete from _data_ttl_sorted, it would delete after removeOneExpiredEntry automatically.
    _data_ttl_sorted.insert({key, end_time});
  }

  // Удаляет запись по ключу key.
  // Возвращает true, если запись была удалена. Если ключа не было до удаления,
  // то вернет false.
  bool remove(std::string_view key) {
    clear_by_ttl();

    std::unique_lock lock(mutex);

    return _data.remove(std::string(key));
  }

  // Получает значение по ключу key. Если данного ключа нет, то вернет
  // std::nullopt.
  std::optional<std::string> get(std::string_view key) {
    clear_by_ttl();

    std::shared_lock lock(mutex);

    auto res = _data.find(std::string(key));
    if (res == nullptr) {
      return std::nullopt;
    }
    return {value(res)};
  }

  // Возвращает следующие count записей начиная с key в порядке
  // лексикографической сортировки ключей. Пример: ("a", "val1"), ("b", "val2"),
  // ("d", "val3"), ("e", "val4") getManySorted("c", 2) -> ("d", "val3"), ("e",
  // "val4")
  std::vector<std::pair<std::string, std::string>>
  getManySorted(std::string_view key, uint32_t count) {
    clear_by_ttl();

    std::shared_lock lock(mutex);

    auto res = _data.getManySorted(key, count);
    return res;
  }

  // Удаляет протухшую запись из структуры и возвращает ее. Если удалять нечего,
  // то вернет std::nullopt. Если на момент вызова метода протухло несколько
  // записей, то можно удалить любую.
  std::optional<std::pair<std::string, std::string>> removeOneExpiredEntry() {
    std::unique_lock lock(mutex);

    // erase everything that is not relevant
    while(ttl(_data.find(_data_ttl_sorted.begin()->key)) != _data_ttl_sorted.begin()->ttl) {
      _data_ttl_sorted.erase(_data_ttl_sorted.begin());
    }

    auto first = _data_ttl_sorted.begin();
    if (first->ttl < get_time()) {
      auto f = _data.find(first->key);
      std::pair<std::string, std::string> res = {key(f), value(f)};
      remove(first->key);
      return {res};
    }
    return std::nullopt;
  }

  uint32_t size() {
    return _data.size();
  }

private:
  void clear_by_ttl() {
    while(size() > 0 && _data_ttl_sorted.begin()->ttl < get_time()) {
      removeOneExpiredEntry();
    }
  }

  // TODO: maybe clock.now() -> is not a proper way to get current time x)
  uint64_t get_time() { return _clock.now(); }

  uint64_t get_end_time(uint64_t add) {
    if(add == 0) {
      return -1;
    } 
    return get_time() + add;
  }

  mutable std::shared_mutex mutex;
  detail::Treap _data;
  std::set<detail::KeyNode, detail::KeyNodeComparator> _data_ttl_sorted;
  Clock _clock;
};