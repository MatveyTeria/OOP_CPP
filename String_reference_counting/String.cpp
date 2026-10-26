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



