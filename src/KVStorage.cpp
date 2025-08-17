#include "KVStorage.h"

namespace detail {

uint64_t DefaultClock::now() const {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

bool KeyNodeComparator::operator()(const KeyNode &a, const KeyNode &b) const {
  if (a.ttl == b.ttl) {
    return a.key < b.key;
  }
  return a.ttl < b.ttl;
}

} // namespace detail