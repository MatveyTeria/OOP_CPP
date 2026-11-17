#pragma once

#include "core.h"
#include <cstdint>
#include <map>
#include <utility>

namespace ser {

template <typename K, typename V>
struct serializer<std::map<K, V>> {
    static void apply(const std::map<K, V>& obj, std::ostream& os) {
        const std::uint64_t size = static_cast<std::uint64_t>(obj.size());
        serializer<std::uint64_t>::apply(size, os);
        for (const auto& kv : obj) {
            serializer<K>::apply(kv.first, os);
            serializer<V>::apply(kv.second, os);
        }
    }
};

template <typename K, typename V>
struct deserializer<std::map<K, V>> {
    static void apply(std::map<K, V>& val, std::istream& is) {
        std::uint64_t size = 0;
        deserializer<std::uint64_t>::apply(size, is);
        if (!is) { val.clear(); return; }

        val.clear();
        for (std::uint64_t i = 0; i < size; ++i) {
            K key{};
            V value{};
            deserializer<K>::apply(key, is);
            deserializer<V>::apply(value, is);
            if (!is) { val.clear(); return; }
            val.emplace(std::move(key), std::move(value));
        }
    }
};

} // namespace ser