#include <gtest/gtest.h>

#include <cstring>
#include <stdexcept>
#include <string>

#include "String.hpp"

namespace {

// Базовые инварианты любого корректно сконструированного String.
void check_invariants(const String& s) {
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(s.data()[s.size()], '\0');
    EXPECT_GE(s.capacity(), s.size());
    EXPECT_GE(s.countRef(), 1u);
}

// Свежий объект — единственный владелец блока.
void check_fresh(const String& s) {
    check_invariants(s);
    EXPECT_EQ(s.countRef(), 1u);
}

}  // namespace

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

TEST(StringSubstr, SubstrBasic) {
    String src("hello world");
    String s(src, 0, 5);
    EXPECT_EQ(s.compare("hello"), 0);
    check_fresh(s);
}

TEST(StringSubstr, SubstrToEndWithNpos) {
    String src("hello world");
    String s(src, 6);
    EXPECT_EQ(s.compare("world"), 0);
    check_fresh(s);
}

TEST(StringSubstr, SubstrLenExceedsRemainder) {
    String src("hello world");
    String s(src, 6, 100);
    EXPECT_EQ(s.compare("world"), 0);
    check_fresh(s);
}

TEST(StringSubstr, SubstrPosEqualsSizeIsEmpty) {
    String src("hello");
    String s(src, 5);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.data()[0], '\0');
    check_fresh(s);
}

TEST(StringSubstr, SubstrLenZeroIsEmpty) {
    String src("hello");
    String s(src, 2, 0);
    EXPECT_TRUE(s.empty());
    check_fresh(s);
}

TEST(StringSubstr, SubstrPosGreaterThanSizeThrows) {
    String src("hello");
    EXPECT_THROW(String(src, 6), std::out_of_range);
    EXPECT_THROW(String(src, 100, 5), std::out_of_range);
}

TEST(StringSubstr, SubstrDoesNotShareBufferWithSource) {
    String src("hello world");
    String s(src, 0, 5);
    EXPECT_EQ(src.countRef(), 1u);
    EXPECT_EQ(s.countRef(), 1u);
    EXPECT_NE(src.data(), s.data());
}

TEST(StringSubstr, SubstrFromEmpty) {
    String empty_src;
    String s(empty_src, 0);
    EXPECT_TRUE(s.empty());
    check_fresh(s);
    EXPECT_THROW(String(empty_src, 1), std::out_of_range);
}

TEST(StringSubstr, SubstrKeepsEmbeddedNul) {
    String src("a\0b\0c", 5);
    String s(src, 1, 3);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s[0], '\0');
    EXPECT_EQ(s[1], 'b');
    EXPECT_EQ(s[2], '\0');
    EXPECT_EQ(s.data()[3], '\0');
    check_fresh(s);
}

// ---------- operator+=(const String&) ----------

TEST(StringAppend, AppendString) {
    String a("hello");
    String b(" world");
    a += b;
    EXPECT_EQ(a.compare("hello world"), 0);
    check_fresh(a);
    // b не должна измениться
    EXPECT_EQ(b.compare(" world"), 0);
}

TEST(StringAppend, AppendEmptyString) {
    String a("hello");
    String empty;
    a += empty;
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringAppend, AppendToEmpty) {
    String a;
    a += String("hello");
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringAppend, AppendStringDetachesBuffer) {
    String a("hello");
    String b = a;                       // COW, refCount == 2
    ASSERT_EQ(a.countRef(), 2u);

    a += String(" world");
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
    EXPECT_EQ(a.compare("hello world"), 0);
    EXPECT_EQ(b.compare("hello"), 0);   // b не изменилась
    check_invariants(a);
    check_invariants(b);
}

TEST(StringAppend, SelfAppend) {
    String a("ab");
    a += a;
    EXPECT_EQ(a.compare("abab"), 0);
    check_fresh(a);
}

TEST(StringAppend, SelfAppendLargeCausesReallocation) {
    String a(100, 'x');
    a += a;
    EXPECT_EQ(a.size(), 200u);
    EXPECT_EQ(a[0], 'x');
    EXPECT_EQ(a[99], 'x');
    EXPECT_EQ(a[100], 'x');
    EXPECT_EQ(a[199], 'x');
    check_fresh(a);
}

TEST(StringAppend, SelfAppendWhenSharedWithAnother) {
    String a("xy");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a += a;
    EXPECT_EQ(a.compare("xyxy"), 0);
    EXPECT_EQ(b.compare("xy"), 0);
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
}

TEST(StringAppend, AppendCString) {
    String a("hello");
    a += " world";
    EXPECT_EQ(a.compare("hello world"), 0);
    check_fresh(a);
}

TEST(StringAppend, AppendEmptyCString) {
    String a("hello");
    a += "";
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringAppend, AppendCStringToEmpty) {
    String a;
    a += "hello";
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringAppend, AppendNullCStringThrows) {
    String a("hello");
    EXPECT_THROW(a += static_cast<const char*>(nullptr), std::invalid_argument);
}

TEST(StringAppend, AppendCStringDetachesBuffer) {
    String a("hello");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a += "!";
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
    EXPECT_EQ(a.compare("hello!"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
}

TEST(StringAppend, AppendChar) {
    String a("hello");
    a += '!';
    EXPECT_EQ(a.compare("hello!"), 0);
    check_fresh(a);
}

TEST(StringAppend, AppendCharToEmpty) {
    String a;
    a += 'x';
    EXPECT_EQ(a.size(), 1u);
    EXPECT_EQ(a[0], 'x');
    EXPECT_EQ(a.data()[1], '\0');
    check_fresh(a);
}

TEST(StringAppend, AppendEmbeddedNulChar) {
    String a("ab");
    a += '\0';
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a[0], 'a');
    EXPECT_EQ(a[1], 'b');
    EXPECT_EQ(a[2], '\0');
    EXPECT_EQ(a.data()[3], '\0');
    check_fresh(a);
}

TEST(StringAppend, AppendCharDetachesBuffer) {
    String a("hello");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a += '!';
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
    EXPECT_EQ(a.compare("hello!"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
}

TEST(StringAppend, ReturnsReferenceForChaining) {
    String a;
    (a += "a") += "b";
    (a += 'c') += "d";
    EXPECT_EQ(a.compare("abcd"), 0);
    check_fresh(a);
}

TEST(StringAppend, ChainingPreservesCOW) {
    String a("x");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    (a += "y") += "z";
    EXPECT_EQ(a.compare("xyz"), 0);
    EXPECT_EQ(b.compare("x"), 0);
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
}

TEST(StringAppend, AppendStringWithEmbeddedNul) {
    String a("a\0b", 3);
    String b("\0c", 2);
    a += b;
    EXPECT_EQ(a.size(), 5u);
    EXPECT_EQ(a[0], 'a');
    EXPECT_EQ(a[1], '\0');
    EXPECT_EQ(a[2], 'b');
    EXPECT_EQ(a[3], '\0');
    EXPECT_EQ(a[4], 'c');
    EXPECT_EQ(a.data()[5], '\0');
    check_fresh(a);
}

TEST(StringAppend, AppendCStringStopsAtNul) {
    String a("ab");
    a += "cd\0ef";
    EXPECT_EQ(a.size(), 4u);
    EXPECT_EQ(a.compare("abcd"), 0);
    check_fresh(a);
}

TEST(StringAppend, ManyAppendsGrowCapacity) {
    String a;
    for (int i = 0; i < 100; ++i)
        a += 'x';
    EXPECT_EQ(a.size(), 100u);
    for (std::size_t i = 0; i < 100; ++i)
        EXPECT_EQ(a[i], 'x');
    check_fresh(a);
}

TEST(StringAppend, AppendStringToItselfThroughCopy) {
    String a("hello");
    String b = a;
    a += b;
    EXPECT_EQ(a.compare("hellohello"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
    check_fresh(a);
}

// ---------- insert(pos, const String&) ----------

TEST(StringInsert, InsertAtBeginning) {
    String a("hello");
    a.insert(0, String("XYZ"));
    EXPECT_EQ(a.compare("XYZhello"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertInMiddle) {
    String a("hello");
    a.insert(2, String("XYZ"));
    EXPECT_EQ(a.compare("heXYZllo"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertAtEnd) {
    String a("hello");
    a.insert(5, String("XYZ"));         // pos == size
    EXPECT_EQ(a.compare("helloXYZ"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertEmptyString) {
    String a("hello");
    a.insert(2, String());
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertIntoEmpty) {
    String a;
    a.insert(0, String("hello"));
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertPosGreaterThanSizeThrows) {
    String a("hello");
    EXPECT_THROW(a.insert(6, String("x")), std::out_of_range);
}

TEST(StringInsert, InsertStringDetachesBuffer) {
    String a("hello");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a.insert(0, String("!"));
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
    EXPECT_EQ(a.compare("!hello"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
    check_invariants(a);
    check_invariants(b);
}

TEST(StringInsert, SelfInsert) {
    String a("ab");
    a.insert(1, a);
    EXPECT_EQ(a.compare("aabb"), 0);
    check_fresh(a);
}

TEST(StringInsert, SelfInsertLargeCausesReallocation) {
    String a(100, 'x');
    a.insert(50, a);
    EXPECT_EQ(a.size(), 200u);
    EXPECT_EQ(a[49], 'x');
    EXPECT_EQ(a[50], 'x');
    EXPECT_EQ(a[51], 'x');
    check_fresh(a);
}

// ---------- insert(pos, const char*) ----------

TEST(StringInsert, InsertCString) {
    String a("hello");
    a.insert(2, "XYZ");
    EXPECT_EQ(a.compare("heXYZllo"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertEmptyCString) {
    String a("hello");
    a.insert(2, "");
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertCStringAtEnd) {
    String a("hello");
    a.insert(5, "!");
    EXPECT_EQ(a.compare("hello!"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertNullCStringThrows) {
    String a("hello");
    EXPECT_THROW(a.insert(0, static_cast<const char*>(nullptr)),
                 std::invalid_argument);
}

TEST(StringInsert, InsertCStringPosGreaterThanSizeThrows) {
    String a("hello");
    EXPECT_THROW(a.insert(6, "x"), std::out_of_range);
}

TEST(StringInsert, InsertCStringDetachesBuffer) {
    String a("hello");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a.insert(0, ">");
    EXPECT_EQ(a.compare(">hello"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
}

// ---------- insert(pos, n, char) ----------

TEST(StringInsert, InsertNChars) {
    String a("hello");
    a.insert(2, 3, 'x');
    EXPECT_EQ(a.compare("hexxxllo"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertNCharsAtEnd) {
    String a("hello");
    a.insert(5, 3, 'x');
    EXPECT_EQ(a.compare("helloxxx"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertNCharsAtBeginning) {
    String a("hello");
    a.insert(0, 3, 'x');
    EXPECT_EQ(a.compare("xxxhello"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertZeroChars) {
    String a("hello");
    a.insert(2, 0, 'x');
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringInsert, InsertNCharsPosGreaterThanSizeThrows) {
    String a("hello");
    EXPECT_THROW(a.insert(6, 1, 'x'), std::out_of_range);
}

TEST(StringInsert, InsertEmbeddedNulChar) {
    String a("ab");
    a.insert(1, 2, '\0');
    EXPECT_EQ(a.size(), 4u);
    EXPECT_EQ(a[0], 'a');
    EXPECT_EQ(a[1], '\0');
    EXPECT_EQ(a[2], '\0');
    EXPECT_EQ(a[3], 'b');
    EXPECT_EQ(a.data()[4], '\0');
    check_fresh(a);
}

TEST(StringInsert, InsertNCharsDetachesBuffer) {
    String a("hello");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a.insert(0, 2, '!');
    EXPECT_EQ(a.compare("!!hello"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
}

// ---------- insert возвращает ссылку на *this ----------

TEST(StringInsert, ReturnsReferenceForChaining) {
    String a("mid");
    (a.insert(0, "<")) . insert(a.size(), ">");
    EXPECT_EQ(a.compare("<mid>"), 0);
    check_fresh(a);
}

// ---------- Встроенные '\0' ----------

TEST(StringInsert, InsertStringWithEmbeddedNul) {
    String a("ab");
    String b("x\0y", 3);
    a.insert(1, b);
    EXPECT_EQ(a.size(), 5u);
    EXPECT_EQ(a[0], 'a');
    EXPECT_EQ(a[1], 'x');
    EXPECT_EQ(a[2], '\0');
    EXPECT_EQ(a[3], 'y');
    EXPECT_EQ(a[4], 'b');
    EXPECT_EQ(a.data()[5], '\0');
    check_fresh(a);
}

// ---------- Сохранение содержимого при перевыделении ----------

TEST(StringInsert, LargeInsertPreservesData) {
    String a("hello");
    a.insert(2, String(1000, 'x'));
    EXPECT_EQ(a.size(), 1005u);
    EXPECT_EQ(a[0], 'h');
    EXPECT_EQ(a[1], 'e');
    EXPECT_EQ(a[2], 'x');
    EXPECT_EQ(a[1001], 'x');
    EXPECT_EQ(a[1002], 'l');
    EXPECT_EQ(a[1003], 'l');
    EXPECT_EQ(a[1004], 'o');
    EXPECT_EQ(a.data()[1005], '\0');
    check_fresh(a);
}

// ---------- Границы ----------

TEST(StringInsert, InsertAtEveryPosition) {
    for (std::size_t pos = 0; pos <= 5; ++pos) {
        String a("hello");
        a.insert(pos, "!");
        EXPECT_EQ(a.size(), 6u) << "pos=" << pos;
        EXPECT_EQ(a[pos], '!') << "pos=" << pos;
        EXPECT_EQ(a.data()[6], '\0') << "pos=" << pos;
        check_fresh(a);
    }
}

TEST(StringInsert, InsertAtEveryPositionWithString) {
    for (std::size_t pos = 0; pos <= 5; ++pos) {
        String a("hello");
        a.insert(pos, String("XY"));
        EXPECT_EQ(a.size(), 7u) << "pos=" << pos;
        EXPECT_EQ(a[pos], 'X') << "pos=" << pos;
        EXPECT_EQ(a[pos + 1], 'Y') << "pos=" << pos;
        EXPECT_EQ(a.data()[7], '\0') << "pos=" << pos;
        check_fresh(a);
    }
}

// ---------- erase(pos, len) ----------

TEST(StringErase, EraseFromMiddle) {
    String a("hello");
    a.erase(1, 2);
    EXPECT_EQ(a.compare("hlo"), 0);
    check_fresh(a);
}

TEST(StringErase, EraseFromBeginning) {
    String a("hello");
    a.erase(0, 2);
    EXPECT_EQ(a.compare("llo"), 0);
    check_fresh(a);
}

TEST(StringErase, EraseFromEndWithLen) {
    String a("hello");
    a.erase(3, 10);                     // len > остатка — обрезаем до конца
    EXPECT_EQ(a.compare("hel"), 0);
    check_fresh(a);
}

TEST(StringErase, EraseToEndWithNpos) {
    String a("hello");
    a.erase(2);                         // len = npos по умолчанию
    EXPECT_EQ(a.compare("he"), 0);
    EXPECT_EQ(a.data()[2], '\0');
    check_fresh(a);
}

TEST(StringErase, EraseAll) {
    String a("hello");
    a.erase(0);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.data()[0], '\0');
    check_fresh(a);
}

TEST(StringErase, ErasePosEqualsSizeIsNoOp) {
    String a("hello");
    a.erase(5);                         // pos == size — допустимо
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringErase, EraseLenZeroIsNoOp) {
    String a("hello");
    a.erase(2, 0);
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

TEST(StringErase, EraseFromEmpty) {
    String a;
    a.erase(0);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.data()[0], '\0');
    check_fresh(a);
}

TEST(StringErase, ErasePosGreaterThanSizeThrows) {
    String a("hello");
    EXPECT_THROW(a.erase(6), std::out_of_range);
    EXPECT_THROW(a.erase(100, 5), std::out_of_range);
}

TEST(StringErase, EraseDetachesBuffer) {
    String a("hello");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);

    a.erase(0, 1);
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
    EXPECT_EQ(a.compare("ello"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
    check_invariants(a);
    check_invariants(b);
}

TEST(StringErase, EraseEveryPossibleRange) {
    // Полный перебор: удаляем [pos, pos+len) из "abcdef" и сверяем с эталоном
    const std::string base = "abcdef";
    for (std::size_t pos = 0; pos <= base.size(); ++pos) {
        for (std::size_t len = 0; len <= base.size() + 2; ++len) {
            String a(base.c_str());
            a.erase(pos, len);
            std::string expected = base;
            expected.erase(pos, len);   // std::string::erase сам нормализует len
            EXPECT_EQ(a.compare(expected.c_str()), 0)
                << "pos=" << pos << " len=" << len;
            check_fresh(a);
        }
    }
}

TEST(StringErase, EraseShrinksSizeButNotCapacity) {
    String a("hello world");
    a.reserve(100);
    std::size_t cap = a.capacity();
    a.erase(0, 5);
    EXPECT_EQ(a.size(), 6u);
    EXPECT_GE(a.capacity(), cap);       // erase не уменьшает ёмкость
    EXPECT_EQ(a.compare(" world"), 0);
}

// ---------- swap: базовое ----------

TEST(StringSwap, SwapsContents) {
    String a("hello");
    String b("world");
    a.swap(b);
    EXPECT_EQ(a.compare("world"), 0);
    EXPECT_EQ(b.compare("hello"), 0);
    check_fresh(a);
    check_fresh(b);
}

TEST(StringSwap, SwapsDifferentLengths) {
    String a("a");
    String b("longer string");
    a.swap(b);
    EXPECT_EQ(a.compare("longer string"), 0);
    EXPECT_EQ(b.compare("a"), 0);
    check_invariants(a);
    check_invariants(b);
}

TEST(StringSwap, SwapsEmptyAndNonEmpty) {
    String a;
    String b("x");
    a.swap(b);
    EXPECT_EQ(a.compare("x"), 0);
    EXPECT_TRUE(b.empty());
    check_invariants(a);
    check_invariants(b);
}

TEST(StringSwap, SwapsTwoEmpty) {
    String a;
    String b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    EXPECT_TRUE(b.empty());
    check_invariants(a);
    check_invariants(b);
}

TEST(StringSwap, SelfSwapIsNoOp) {
    String a("hello");
    a.swap(a);
    EXPECT_EQ(a.compare("hello"), 0);
    check_fresh(a);
}

// ---------- swap: refCount не меняется ----------

TEST(StringSwap, DoesNotChangeRefCountOfBlocks) {
    // Блок X: refCount 2 (a, b)
    // Блок Y: refCount 1 (c)
    String a("shared");
    String b = a;
    ASSERT_EQ(a.countRef(), 2u);
    ASSERT_EQ(b.countRef(), 2u);

    String c("other");
    ASSERT_EQ(c.countRef(), 1u);

    a.swap(c);
    // a -> Y (refCount 1)
    // c -> X (refCount 2: c, b)
    // b -> X (refCount 2)
    EXPECT_EQ(a.countRef(), 1u);
    EXPECT_EQ(c.countRef(), 2u);
    EXPECT_EQ(b.countRef(), 2u);
    EXPECT_EQ(a.compare("other"), 0);
    EXPECT_EQ(c.compare("shared"), 0);
    EXPECT_EQ(b.compare("shared"), 0);
}

TEST(StringSwap, SwapThenWriteDetachesCorrectly) {
    String a("shared");
    String b = a;                       // X: refCount 2 (a, b)
    String c("other");                  // Y: refCount 1 (c)

    a.swap(c);                          // a -> Y, c -> X (refCount 2: c, b)

    a[0] = 'O';                         // a — единственный владелец Y, detach no-op
    EXPECT_EQ(a.compare("Other"), 0);
    EXPECT_EQ(c.compare("shared"), 0);
    EXPECT_EQ(b.compare("shared"), 0);

    c[0] = 'S';                         // c делит X с b — detach
    EXPECT_EQ(c.compare("Shared"), 0);
    EXPECT_EQ(b.compare("shared"), 0);  // b не изменилась
    EXPECT_EQ(c.countRef(), 1u);
    EXPECT_EQ(b.countRef(), 1u);
}

// ---------- swap: время жизни ----------

TEST(StringSwap, SwapWithTemporaryBlock) {
    // После swap старый блок *this должен корректно освободиться,
    // когда умрёт второй объект
    String a("x");
    {
        String b("long string with big capacity");
        b.reserve(1000);
        a.swap(b);
        EXPECT_EQ(a.compare("long string with big capacity"), 0);
        EXPECT_EQ(b.compare("x"), 0);
    }
    EXPECT_EQ(a.compare("long string with big capacity"), 0);
    check_fresh(a);
}

// ---------- swap: встроенные '\0' ----------

TEST(StringSwap, SwapsEmbeddedNul) {
    String a("a\0b", 3);
    String b("x\0y", 3);
    a.swap(b);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a[0], 'x');
    EXPECT_EQ(a[1], '\0');
    EXPECT_EQ(a[2], 'y');
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[0], 'a');
    EXPECT_EQ(b[1], '\0');
    EXPECT_EQ(b[2], 'b');
    check_invariants(a);
    check_invariants(b);
}

// ---------- find(char) ----------

TEST(StringFind, FindCharBasic) {
    String s("hello world");
    EXPECT_EQ(s.find('h'), 0u);
    EXPECT_EQ(s.find('e'), 1u);
    EXPECT_EQ(s.find('o'), 4u);         // первое 'o'
    EXPECT_EQ(s.find('d'), 10u);
}

TEST(StringFind, FindCharFromPos) {
    String s("hello world");
    EXPECT_EQ(s.find('o', 5), 7u);      // второе 'o'
    EXPECT_EQ(s.find('l', 3), 3u);
    EXPECT_EQ(s.find('h', 1), String::npos);  // 'h' только в 0
}

TEST(StringFind, FindCharNotFound) {
    String s("hello");
    EXPECT_EQ(s.find('z'), String::npos);
    EXPECT_EQ(s.find('x', 0), String::npos);
}

TEST(StringFind, FindCharPosEqualsSizeReturnsNpos) {
    String s("hello");
    EXPECT_EQ(s.find('o', 5), String::npos);  // pos == size — искать негде
    EXPECT_EQ(s.find('o', 100), String::npos); // pos > size — тоже npos
}

TEST(StringFind, FindCharInEmptyString) {
    String s;
    EXPECT_EQ(s.find('a'), String::npos);
    EXPECT_EQ(s.find('a', 0), String::npos);
    EXPECT_EQ(s.find('a', 1), String::npos);
}

TEST(StringFind, FindEmbeddedNulChar) {
    String s("a\0b\0c", 5);
    EXPECT_EQ(s.find('\0'), 1u);        // первое '\0'
    EXPECT_EQ(s.find('\0', 2), 3u);     // второе '\0'
    EXPECT_EQ(s.find('\0', 4), String::npos);
}

TEST(StringFind, FindCharAtLastPosition) {
    String s("abc");
    EXPECT_EQ(s.find('c'), 2u);
    EXPECT_EQ(s.find('c', 2), 2u);
    EXPECT_EQ(s.find('c', 3), String::npos);
}