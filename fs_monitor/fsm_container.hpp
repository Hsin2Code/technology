#ifndef _FSM_CONTAINER_HPP
#define _FSM_CONTAINER_HPP

#if __cplusplus > 201103L

#define HAVE_UNORDERED_MAP
#define HAVE_UNORDERED_SET

#endif

#if defined(HAVE_UNORDERED_MAP)
#include <unordered_map>

namespace fsm
{
    template<typename K, typename V>
    using fsm_hash_map = std::unordered_map<K, V>;
}

#else
#include <map>

namespace fsm
{
    template <typename K, typename V>
    using fsm_hash_map = std::map<K, V>;
}

#endif


#if defined(HAVE_UNORDERED_SET)
#include <unordered_set>

namespace fsm
{
  template<typename K>
  using fsm_hash_set = std::unordered_set<K>;
}

#else
#include <set>

namespace fsm
{
  template <typename K>
  using fsm_hash_set = std::set<K>;
}

#endif

#endif  // _FSM_CONTAINER_HPP
