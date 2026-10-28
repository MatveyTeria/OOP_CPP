#include <gtest/gtest.h>
#include <sstream>

#include "String.h"

TEST(StringCtor, DefaultIsEmpty) {
    String s;
    EXPECT_EQ(s.size(), 0u);
    EXPECT_TRUE(s.empty());
    check_fresh(s);
}

TEST(StringCtor, DefaultDataIsValidCString) {
    String s;
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(s.data()[0], '\0');
    EXPECT_EQ(std::strlen(s.data()), 0u);
}

TEST(StringCtor, DefaultBehavesLikeOrdinaryEmptyString) {
    String s;
    EXPECT_EQ(s.find('a'), String::npos);
    EXPECT_EQ(s.find(""), 0u);
    EXPECT_EQ(s.compare(""), 0);
    EXPECT_TRUE(s.substr(0).empty());
}

// ---------- String(const char*) ----------

TEST(StringCtor, FromCString) {
    String s("hello");
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.compare("hello"), 0);
    EXPECT_STREQ(s.data(), "hello");
    check_fresh(s);
}

TEST(StringCtor, FromEmptyCString) {
    String s("");
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.data()[0], '\0');
    check_fresh(s);
}

TEST(StringCtor, FromSingleCharCString) {
    String s("x");
    EXPECT_EQ(s.size(), 1u);
    EXPECT_EQ(s[0], 'x');
    EXPECT_EQ(s.data()[1], '\0');
    check_fresh(s);
}

TEST(StringCtor, FromCStringKeepsSpacesAndPunctuation) {
    String s(" a-b! ");
    EXPECT_EQ(s.size(), 6u);
    EXPECT_EQ(s.compare(" a-b! "), 0);
    check_fresh(s);
}

TEST(StringCtor, NullCStringThrows) {
    EXPECT_THROW(String(static_cast<const char*>(nullptr)),
                 std::invalid_argument);
}

// ---------- String(const char*, size_t) ----------

TEST(StringCtor, FromCStringPrefix) {
    String s("hello world", 5);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.compare("hello"), 0);
    EXPECT_EQ(s.data()[5], '\0');
    check_fresh(s);
}

TEST(StringCtor, FromCStringPrefixZero) {
    String s("hello", 0);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.data()[0], '\0');
    check_fresh(s);
}

TEST(StringCtor, FromCStringPrefixFullLength) {
    String s("hello", 5);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.compare("hello"), 0);
    check_fresh(s);
}

TEST(StringCtor, FromCStringPrefixNullWithZeroIsValid) {
    String s(static_cast<const char*>(nullptr), 0);
    EXPECT_TRUE(s.empty());
    EXPECT_NE(s.data(), nullptr);
    check_fresh(s);
}

TEST(StringCtor, FromCStringPrefixNullWithPositiveThrows) {
    EXPECT_THROW(String(static_cast<const char*>(nullptr), 5),
                 std::invalid_argument);
}

TEST(StringCtor, FromCStringPrefixKeepsEmbeddedNul) {
    const char raw[] = "a\0b\0c";      // 6 байт включая финальный '\0'
    String s(raw, 5);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s[0], 'a');
    EXPECT_EQ(s[1], '\0');
    EXPECT_EQ(s[2], 'b');
    EXPECT_EQ(s[3], '\0');
    EXPECT_EQ(s[4], 'c');
    EXPECT_EQ(s.data()[5], '\0');
    check_fresh(s);
}

// ---------- String(size_t, char) ----------

TEST(StringCtor, FillWithChar) {
    String s(5, 'x');
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.compare("xxxxx"), 0);
    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(s[i], 'x');
    EXPECT_EQ(s.data()[5], '\0');
    check_fresh(s);
}

TEST(StringCtor, FillZeroTimesIsEmpty) {
    String s(0, 'x');
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.data()[0], '\0');
    check_fresh(s);
}

TEST(StringCtor, FillOnce) {
    String s(1, 'a');
    EXPECT_EQ(s.size(), 1u);
    EXPECT_EQ(s[0], 'a');
    EXPECT_EQ(s.data()[1], '\0');
    check_fresh(s);
}

TEST(StringCtor, FillWithEmbeddedNul) {
    String s(3, '\0');
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s[0], '\0');
    EXPECT_EQ(s[1], '\0');
    EXPECT_EQ(s[2], '\0');
    EXPECT_EQ(s.data()[3], '\0');
    EXPECT_EQ(std::strlen(s.data()), 0u);   // C-строка пустая, но size == 3
    check_fresh(s);
}

TEST(StringCtor, FillWithSpace) {
    String s(4, ' ');
    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s.compare("    "), 0);
    check_fresh(s);
}

// ---------- String(const String&) — COW ----------

TEST(StringCtor, CopyIsCow) {
    String a("hello");
    ASSERT_EQ(a.countRef(), 1u);

    String b = a;
    EXPECT_EQ(a.countRef(), 2u);
    EXPECT_EQ(b.countRef(), 2u);
    EXPECT_EQ(a.compare("hello"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
}

TEST(StringCtor, CopyDetachesOnWrite) {
    String a("hello");
    String b = a;

    b[0] = 'H';
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
    EXPECT_EQ(a.compare("hello"), 0);
    EXPECT_EQ(b.compare("Hello"), 0);
}

TEST(StringCtor, CopyChainIncrementsRefCount) {
    String a("x");
    String b = a;
    String c = b;
    EXPECT_EQ(a.countRef(), 3u);
    EXPECT_EQ(b.countRef(), 3u);
    EXPECT_EQ(c.countRef(), 3u);
}

TEST(StringCtor, DestroyingCopyDecrementsRefCount) {
    String a("x");
    {
        String tmp = a;
        EXPECT_EQ(a.countRef(), 2u);
    }
    EXPECT_EQ(a.countRef(), 1u);
}

TEST(StringCtor, CopyOfEmpty) {
    String a;
    String b = a;
    EXPECT_EQ(a.countRef(), 2u);
    EXPECT_TRUE(b.empty());
    check_invariants(b);
}

// ---------- String(const String&, pos, len) ----------

TEST(StringCtor, SubstrBasic) {
    String src("hello world");
    String s(src, 0, 5);
    EXPECT_EQ(s.compare("hello"), 0);
    check_fresh(s);
}

TEST(StringCtor, SubstrToEndWithNpos) {
    String src("hello world");
    String s(src, 6);
    EXPECT_EQ(s.compare("world"), 0);
    check_fresh(s);
}

TEST(StringCtor, SubstrLenExceedsRemainder) {
    String src("hello world");
    String s(src, 6, 100);
    EXPECT_EQ(s.compare("world"), 0);
    check_fresh(s);
}

TEST(StringCtor, SubstrPosEqualsSizeIsEmpty) {
    String src("hello");
    String s(src, 5);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.data()[0], '\0');
    check_fresh(s);
}

TEST(StringCtor, SubstrLenZeroIsEmpty) {
    String src("hello");
    String s(src, 2, 0);
    EXPECT_TRUE(s.empty());
    check_fresh(s);
}

TEST(StringCtor, SubstrPosGreaterThanSizeThrows) {
    String src("hello");
    EXPECT_THROW(String(src, 6), std::out_of_range);
    EXPECT_THROW(String(src, 100, 5), std::out_of_range);
}

TEST(StringCtor, SubstrDoesNotShareBufferWithSource) {
    String src("hello world");
    String s(src, 0, 5);
    EXPECT_EQ(src.countRef(), 1u);
    EXPECT_EQ(s.countRef(), 1u);
    EXPECT_NE(src.data(), s.data());
}

TEST(StringCtor, SubstrFromEmpty) {
    String empty_src;
    String s(empty_src, 0);
    EXPECT_TRUE(s.empty());
    check_fresh(s);
    EXPECT_THROW(String(empty_src, 1), std::out_of_range);
}

TEST(StringCtor, SubstrKeepsEmbeddedNul) {
    String src("a\0b\0c", 5);
    String s(src, 1, 3);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s[0], '\0');
    EXPECT_EQ(s[1], 'b');
    EXPECT_EQ(s[2], '\0');
    EXPECT_EQ(s.data()[3], '\0');
    check_fresh(s);
}