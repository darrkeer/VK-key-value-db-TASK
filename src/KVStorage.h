#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <vector>

#include "Treap.h"

template <typename T> struct ValueNode {
  T value;
  uint32_t ttl;
};

struct KeyNode {
  std::string key;
  uint32_t ttl;
};

template <typename Clock> class KVStorage {
public:
  // Инициализирует хранилище переданным множеством записей. Размер span может
  // быть очень большим. Также принимает абстракцию часов (Clock) для
  // возможности управления временем в тестах.
  explicit KVStorage(
      std::span<std::tuple<std::string /*key*/, std::string /*value*/,
                           uint32_t /*ttl*/>>
          entries,
      Clock clock = Clock())
      : _clock(std::move(clock)) {
    for (const auto &[k, v, ttl] : entries) {
      set(k, v, ttl);
    }
  }

  ~KVStorage() = default;

  // Присваивает по ключу key значение value.
  // Если ttl == 0, то время жизни записи - бесконечность, иначе запись должна
  // перестать быть доступной через ttl секунд. Безусловно обновляет ttl записи.
  void set(std::string key, std::string value, uint32_t ttl) {}

  // Удаляет запись по ключу key.
  // Возвращает true, если запись была удалена. Если ключа не было до удаления,
  // то вернет false.
  bool remove(std::string_view key) { return false; }

  // Получает значение по ключу key. Если данного ключа нет, то вернет
  // std::nullopt.
  std::optional<std::string> get(std::string_view key) const {
    return std::nullopt;
  }

  // Возвращает следующие count записей начиная с key в порядке
  // лексикографической сортировки ключей. Пример: ("a", "val1"), ("b", "val2"),
  // ("d", "val3"), ("e", "val4") getManySorted("c", 2) -> ("d", "val3"), ("e",
  // "val4")
  std::vector<std::pair<std::string, std::string>>
  getManySorted(std::string_view key, uint32_t count) const {
    // TODO: grab r-lock
  }

  // Удаляет протухшую запись из структуры и возвращает ее. Если удалять нечего,
  // то вернет std::nullopt. Если на момент вызова метода протухло несколько
  // записей, то можно удалить любую.
  std::optional<std::pair<std::string, std::string>> removeOneExpiredEntry() {
    // TODO: grab w-lock

    auto first = _data_ttl_sorted.begin();
    if (first->ttl < get_time()) {
      auto res = get(first->key);
      remove(first->key);
      return res;
    }
    return std::nullopt;
  }

private:
  uint32_t get_time() const { return -1; }

  detail::Treap _data;
  std::set<KeyNode> _data_ttl_sorted;
  Clock _clock;
};