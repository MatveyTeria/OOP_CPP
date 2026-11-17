#pragma once

#include "core.h"
#include <cstdint>
#include <string>

namespace ser {

// std::string содержит внутренний указатель / SSO-буфер — нельзя копировать
// сырые байты sizeof(std::string). Нужна полноценная сериализация.
template <>
struct serializer<std::string> {
    static void apply(const std::string& obj, std::ostream& os) {
        const std::uint64_t size = static_cast<std::uint64_t>(obj.size());
        serializer<std::uint64_t>::apply(size, os);       // префикс длины
        os.write(obj.data(), static_cast<std::streamsize>(obj.size()));
    }
};

template <>
struct deserializer<std::string> {
    static void apply(std::string& val, std::istream& is) {
        std::uint64_t size = 0;
        deserializer<std::uint64_t>::apply(size, is);
        if (!is) { val.clear(); return; }                 // защита от битого входа

        val.resize(static_cast<std::size_t>(size));
        is.read(val.data(), static_cast<std::streamsize>(size));
    }
};

} // namespace ser