#pragma once

#include <cstddef>
#include <istream>
#include <ostream>
#include <type_traits>

namespace ser {

// Primary template: побайтовое копирование для trivially copyable типов.
// Contract: sizeof(T) байт в os / из is, побайтово идентичны памяти.
template <typename T>
struct serializer {
    static_assert(std::is_trivially_copyable_v<T>,
        "serializer<T>: for non-trivially-copyable types provide a specialization");

    static void apply(const T& obj, std::ostream& os) {
        os.write(reinterpret_cast<const char*>(&obj),
                 static_cast<std::streamsize>(sizeof(T)));
    }
};

template <typename T>
struct deserializer {
    static_assert(std::is_trivially_copyable_v<T>,
        "deserializer<T>: for non-trivially-copyable types provide a specialization");

    static void apply(T& val, std::istream& is) {
        is.read(reinterpret_cast<char*>(&val),
                static_cast<std::streamsize>(sizeof(T)));
    }
};

// Free-функции-фасады. Пользователь работает только с ними.
template <typename T>
void serialize(const T& obj, std::ostream& os) {
    serializer<T>::apply(obj, os);
}

template <typename T>
void deserialize(T& obj, std::istream& is) {
    deserializer<T>::apply(obj, is);
}

} // namespace ser