#pragma once

#include "core.h"
#include <cstdint>
#include <vector>

namespace ser {

template <typename T>
struct serializer<std::vector<T>> {
    static void apply(const std::vector<T>& obj, std::ostream& os) {
        const std::uint64_t size = static_cast<std::uint64_t>(obj.size());
        serializer<std::uint64_t>::apply(size, os);
        for (const T& item : obj) {
            serializer<T>::apply(item, os);
        }
    }
};

template <typename T>
struct deserializer<std::vector<T>> {
    static void apply(std::vector<T>& val, std::istream& is) {
        std::uint64_t size = 0;
        deserializer<std::uint64_t>::apply(size, is);
        if (!is) { val.clear(); return; }

        val.resize(static_cast<std::size_t>(size));
        for (T& item : val) {
            deserializer<T>::apply(item, is);
            if (!is) { val.clear(); return; }
        }
    }
};

} // namespace ser