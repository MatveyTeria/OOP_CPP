#include <cstddef>
#include <stdexcept>
#include <memory>
#include <cstring>

#include "String.hpp"

struct String::StringData {
    std::size_t refCount = 1;
    std::size_t capacity = 0;
    std::size_t size     = 0;
    char* data           = nullptr;
}

const char& String::null_char() {
    static const char zero = '\0';
    return zero;
}

String::StringData* String::acquire(const char* src, std::size_t n, std::size_t cap) {
    if (cap < n) cap = n;
    StringData* p(new StringData()); // possible exception - std::bad_alloc
    try {
        p.data = new char[cap + 1];
    } catch(...) {
        delete p;
        throw;
    }
    if (n > 0 && src != nullptr)
        std::memcpy(p->data, src, n);
    p->data[n] = '\0';
    p->size     = n;
    p->capacity = cap;
    p->refCount = 1;
    return p.release();
}

void String::add_ref(StringData* p) {
    ++p->refCount;
}

void String::release(StringData* p) {
    if (p == nullptr) return;
    if (--p->refCount == 0) {
        delete[] p->data;
        delete   p;
    }
}

void String::detach() {
    if (shared_->refCount == 1)
        return;
    
    StringData* freshBuffer = acquire(shared_->data, shared_->size, shared_->capacity);
    release(shared_);
    shared_ = freshBuffer;
}

void String::ensure_capacity(std::size_t n) {
    if (shared_->capacity >= n)
        return;

    detach();

    StringData* fresh = acquire(shared_->data, shared_->size, n);
    release(shared_);
    shared_ = fresh;
}

String::String() : shared_(acquire(nullptr, 0, 0)) {}

String::String(const char* str)
    : shared_(nullptr)
{
    if (str == nullptr)
        throw std::invalid_argument("String: null C-string");
    std::size_t n = std::strlen(str);
    shared_ = acquire(str, n, n);
}

String::String(const char* str, std::size_t n)
    : shared_(nullptr)
{
    if (str == nullptr)
        throw std::invalid_argument("String: null C-string");
    n = std::min(n, std::strlen(str))
    shared_ = acquire(str, n, n);
}

String::String(std::size_t n, char c)
    : shared_(nullptr)
{
    shared_ = acquire(nullptr, 0, n);
    std::memset(shared_->data, c, n);
    shared_->data[n] = '\0';
    shared_->size = n;
}

String::String(const String& str) // Reference counting copy
    : shared_(str.shared_)
{
    add_ref(shared_);
}

String::String(const String& str, std::size_t pos, std::size_t len)
    : shared_(nullptr)
{
    if (pos > str.size())
        throw std::out_of_range("String: pos > size");
    std::size_t n = (len == npos) ? (str.size() - pos) : 
                                    std::min(len, str.size() - pos)
    shared_ = acquire(str.data() + pos, n, n);
}

String::~String() {
    release(shared_);
}

std::size_t String::size() const {
    return shared_->size;
}

std::size_t String::capacity() const {
    return shared_->capacity;
}

bool String::empty() const {
    return shared_->data == nullptr || shared_->size == 0;
}

void String::reserve(std::size_t n) {
    if (n <= shared_->capacity)
        return;
    ensure_capacity(n);
}

void String::clear() {
    detach();
    shared_.size = 0;
    shared_.data[0] = '\0';
}

char& String::at(std::size_t pos) {
    if (pos >= shared_->size)
        throw std::out_of_range("String::at: pos out of range");
    detach();
    return shared_->data[pos];
}

const char& String::at(std::size_t pos) const {
    if (pos > shared_->size)
        throw std::out_of_range("String::at: pos out of range");
    if (pos == shared_->size)
        return null_char();
    return shared_->data[pos];
}

char& String::operator[](std::size_t pos) {
    detach();
    return shared_->data[pos];
}
const char& String::operator[](std::size_t pos) const {
    if (pos == shared_->size)
        return null_char();
    return shared_->data[pos];
}

char& String::back() {
    detach();
    return shared_->data[shared_->size - 1];
}
const char& String::back() const {
    return shared_->data[shared_->size - 1];
}

char& String::front() {
    detach();
    return shared_->data[0];
}
const char& String::front() const {
    return shared_->data[0];
}

String& String::append_raw(const char* src, std::size_t n) {
    if (n == 0)
        return *this;
    detach();
    ensure_capacity(shared_->size + n);
    std::memcpy(shared_->data + shared_->size, src, n);
    shared_->size += n;
    shared_->data[shared_->size] = '\0';
    return *this;
}

String& String::operator+=(const String& str) {
    if (this == &str) {
        String tmp(*this);
        return append_raw(tmp.data(), tmp.size());
    }
    return append_raw(str.data(), str.size());
}

String& String::operator+=(const char* str) {
    if (str == nullptr)
        throw std::invalid_argument("String::operator+=: null C-string");
    return append_raw(str, std::strlen(str));
}

String& String::operator+=(char c) {
    return append_raw(&c, 1);
}

String& String::operator=(const String& str) {
    if (this == str)
        return *this;
    add_ref(std.shared_);
    release(shared_);
    shared_ = str.shared_;
    return *this;
}

String& String::operator=(const char* str) {
    if (str == nullptr)
        throw std::invalid_argument("String::operator=: null C-string");
    String tmp(str);
    swap(tmp);
    return *this;
}

String& String::insert_raw(std::size_t pos, const char* src, std::size_t n) {
    if (pos > shared_->size)
        throw std::out_of_range("String::insert: pos out of range");
    if (n == 0)
        return *this;
    detach();
    ensure_capacity(shared_->size + n);
    char* base = shared_->data;
    std::memmove(base + pos + n, base + pos, shared_->size - pos + 1);
    std::memcpy(base + pos, src, n);

    shared_->size += n;
    return *this;
}

String& String::insert(std::size_t pos, const String& str) {
    if (this == &str) {
        String tmp(str);
        return insert_raw(pos, tmp.data(), tmp.size());
    }
    return insert_raw(pos, str.data(), str.size());
}

String& String::insert(std::size_t pos, const char* str) {
    if (str == nullptr)
        throw std::invalid_argument("String::insert: null C-string");
    return insert_raw(pos, str, std::strlen(str));
}

String& String::insert(std::size_t pos, std::size_t n, char c) {
    if (n == 0)
        return *this;
    if (pos > shared_->size)
        throw std::out_of_range("String::insert: pos out of range");

    detach();
    ensure_capacity(shared_->size + n);

    char* base = shared_->data;
    std::memmove(base + pos + n, base + pos, shared_->size - pos + 1);
    std::memset(base + pos, c, n);

    shared_->size += n;
    return *this;
}

String& String::erase(std::size_t pos, std::size_t len) {
    if (pos > shared_->size)
        throw std::out_of_range("String::erase: pos out of range");

    std::size_t n = (len == npos) ? (shared_->size - pos)
                                  : std::min(len, shared_->size - pos);
    if (n == 0)
        return *this;

    detach();

    char* base = shared_->data;
    std::memmove(base + pos, base + pos + n,
                 shared_->size - pos - n + 1);

    shared_->size -= n;
    return *this;
}

String& String::replace_raw(std::size_t pos, std::size_t len,
                            const char* src, std::size_t n) {
    if (pos > shared_->size)
        throw std::out_of_range("String::replace: pos out of range");

    std::size_t oldLen = (len == npos) ? (shared_->size - pos)
                                       : std::min(len, shared_->size - pos);

    if (oldLen == 0 && n == 0)
        return *this;

    detach();

    std::size_t newSize = shared_->size - oldLen + n;
    ensure_capacity(newSize);

    char* base = shared_->data;

    if (n != oldLen) {
        std::memmove(base + pos + n,
                     base + pos + oldLen,
                     shared_->size - pos - oldLen + 1);
    }

    if (n > 0)
        std::memcpy(base + pos, src, n);

    shared_->size = newSize;
    return *this;
}

String& String::replace(std::size_t pos, std::size_t len, const String& str) {
    if (this == &str) {
        String tmp(str);
        return replace_raw(pos, len, tmp.data(), tmp.size());
    }
    return replace_raw(pos, len, str.data(), str.size());
}

String& String::replace(std::size_t pos, std::size_t len, const char* str) {
    if (str == nullptr)
        throw std::invalid_argument("String::replace: null C-string");
    return replace_raw(pos, len, str, std::strlen(str));
}

String& String::replace(std::size_t pos, std::size_t len, std::size_t n, char c) {
    if (n == 0)
        return erase(pos, len);
    if (pos > shared_->size)
        throw std::out_of_range("String::replace: pos out of range");

    std::size_t oldLen = (len == npos) ? (shared_->size - pos)
                                       : std::min(len, shared_->size - pos);
    detach();
    std::size_t newSize = shared_->size - oldLen + n;
    ensure_capacity(newSize);

    char* base = shared_->data;
    if (n != oldLen) {
        std::memmove(base + pos + n,
                     base + pos + oldLen,
                     shared_->size - pos - oldLen + 1);
    }
    std::memset(base + pos, c, n);
    shared_->size = newSize;
    return *this;
}

// String.cpp
std::size_t String::find_raw(const char* needle, std::size_t n,
                             std::size_t pos) const {
    if (pos > shared_->size)
        return npos;

    if (n == 0)
        return pos;

    if (n > shared_->size - pos)
        return npos;

    const char* hay = shared_->data;
    for (std::size_t i = pos; i <= shared_->size - n; ++i) {
        if (std::memcmp(hay + i, needle, n) == 0)
            return i;
    }
    return npos;
}

std::size_t String::find(char c, std::size_t pos) const {
    if (pos >= shared_->size)
        return npos;
    const char* p = static_cast<const char*>(
        std::memchr(shared_->data + pos, c, shared_->size - pos));
    return p ? static_cast<std::size_t>(p - shared_->data) : npos;
}

std::size_t String::find(const char* str, std::size_t pos) const {
    if (str == nullptr)
        throw std::invalid_argument("String::find: null C-string");
    return find_raw(str, std::strlen(str), pos);
}