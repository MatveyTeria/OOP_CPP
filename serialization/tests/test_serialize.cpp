#include <gtest/gtest.h>

#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include "serialize/serialize.h"

namespace {

template <typename T>
std::string to_bytes(const T& value) {
    std::ostringstream oss(std::ios::out | std::ios::binary);
    ser::serialize(value, oss);
    return oss.str();
}

template <typename T>
T from_bytes(const std::string& bytes) {
    std::istringstream iss(bytes, std::ios::in | std::ios::binary);
    T value{};
    ser::deserialize(value, iss);
    return value;
}

template <typename T>
T roundtrip(const T& value) {
    return from_bytes<T>(to_bytes(value));
}

} // namespace

// ---- Примитивы -------------------------------------------------------

TEST(Serialize, PrimitiveRoundtrip) {
    EXPECT_EQ(roundtrip<int>(42), 42);
    EXPECT_EQ(roundtrip<char>('x'), 'x');
    EXPECT_EQ(roundtrip<std::uint64_t>(1234567890ULL), 1234567890ULL);

    // double: побитовый round-trip точен, сравниваем ==
    const double d = 3.141592653589793;
    EXPECT_EQ(roundtrip<double>(d), d);
}

TEST(Serialize, TrivialArrayRoundtrip) {
    char hello[7] = "hello!";
    int  boo[3]  = {1, 2, 3};

    std::ostringstream oss(std::ios::out | std::ios::binary);
    ser::serialize(hello, oss);
    ser::serialize(boo, oss);

    std::istringstream iss(oss.str(), std::ios::in | std::ios::binary);
    char hello2[7]{};
    int  boo2[3]{};
    ser::deserialize(hello2, iss);
    ser::deserialize(boo2, iss);

    EXPECT_EQ(std::string(hello, 7), std::string(hello2, 7));
    EXPECT_EQ(0, std::memcmp(boo, boo2, sizeof boo));
}

// ---- std::string -----------------------------------------------------

TEST(Serialize, EmptyString) {
    EXPECT_EQ(roundtrip<std::string>(std::string{}), std::string{});
}

TEST(Serialize, ShortString) {
    EXPECT_EQ(roundtrip<std::string>("hello"), "hello");
}

TEST(Serialize, StringWithEmbeddedNulls) {
    const std::string s("abc\0def", 7);
    EXPECT_EQ(roundtrip(s), s);
}

TEST(Serialize, LongString_exceedsSSO) {
    const std::string s(10'000, 'a');
    EXPECT_EQ(roundtrip(s), s);
}

// ---- std::vector<T> --------------------------------------------------

TEST(Serialize, EmptyVector) {
    EXPECT_EQ(roundtrip<std::vector<int>>(std::vector<int>{}),
              std::vector<int>{});
}

TEST(Serialize, IntVector) {
    const std::vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(roundtrip(v), v);
}

TEST(Serialize, StringVector) {
    const std::vector<std::string> v{"one", "two", "three", ""};
    EXPECT_EQ(roundtrip(v), v);
}

TEST(Serialize, NestedVectors) {
    const std::vector<std::vector<int>> v{{1, 2}, {}, {3, 4, 5}};
    EXPECT_EQ(roundtrip(v), v);
}

// ---- std::map<K, V> --------------------------------------------------

TEST(Serialize, EmptyMap) {
    using MapSI = std::map<std::string, int>;
    EXPECT_EQ(roundtrip<MapSI>(MapSI{}), MapSI{});
}

TEST(Serialize, StringIntMap) {
    const std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}};
    EXPECT_EQ(roundtrip(m), m);
}

TEST(Serialize, MapOfVectors) {
    const std::map<std::string, std::vector<int>> m{
        {"a", {1, 2}}, {"b", {}}, {"c", {3, 4, 5}}};
    EXPECT_EQ(roundtrip(m), m);
}

TEST(Serialize, DeserializeOverwritesExistingValue) {
    std::map<std::string, int> target{{"stale", 99}};
    const auto bytes = to_bytes(std::map<std::string, int>{{"fresh", 1}});
    std::istringstream iss(bytes, std::ios::in | std::ios::binary);
    ser::deserialize(target, iss);
    EXPECT_EQ(target, (std::map<std::string, int>{{"fresh", 1}}));
}

// ---- Композиция формата ---------------------------------------------

TEST(Serialize, MultipleValuesInOneStream) {
    std::ostringstream oss(std::ios::out | std::ios::binary);
    const std::string s = "hi";
    const std::vector<int> v{7, 8, 9};
    const std::uint64_t n = 123;

    ser::serialize(s, oss);
    ser::serialize(v, oss);
    ser::serialize(n, oss);

    std::istringstream iss(oss.str(), std::ios::in | std::ios::binary);
    std::string s2;
    std::vector<int> v2;
    std::uint64_t n2 = 0;

    ser::deserialize(s2, iss);
    ser::deserialize(v2, iss);
    ser::deserialize(n2, iss);

    EXPECT_EQ(s2, s);
    EXPECT_EQ(v2, v);
    EXPECT_EQ(n2, n);
    EXPECT_TRUE(iss.eof() || iss.good());  // весь формат прочитан
}

// ---- Поведение на битом входе --------------------------------------

TEST(Serialize, BrokenInputDoesNotThrow) {
    // Строка длины 100, но данных только 3 байта.
    std::string bytes = to_bytes<std::uint64_t>(100) + "abc";
    std::istringstream iss(bytes, std::ios::in | std::ios::binary);
    std::string s;
    EXPECT_NO_THROW(ser::deserialize(s, iss));
    EXPECT_TRUE(iss.fail());
}