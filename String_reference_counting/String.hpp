#pragma once

#include <cstddef>
#include <stdexcept>

class String {
public:
    String();
    String(const char* str);
    String(const char* str, std::size_t n);
    String(std::size_t n, char c);
    String(const String& str);
    String(const String& str, std::size_t pos, std::size_t len = npos);
    virtual ~String();

    std::size_t size() const;
    std::size_t capacity() const;
    void   reserve(std::size_t n = 0);
    void   clear();
    bool   empty() const;

    // ---------- Доступ к символам ----------
    char&       at(std::size_t pos);
    const char& at(std::size_t pos) const;

    char&       operator[](std::size_t pos);
    const char& operator[](std::size_t pos) const;

    char&       back();
    const char& back() const;

    char&       front();
    const char& front() const;

    // ---------- Конкатенация ----------
    String& operator+=(const String& str);
    String& operator+=(const char* str);
    String& operator+=(char c);

    // ---------- Присваивание ----------
    String& operator=(const String& str);
    String& operator=(const char* str);

    // ---------- Вставка ----------
    String& insert(std::size_t pos, const String& str);
    String& insert(std::size_t pos, const char* str);
    String& insert(std::size_t pos, std::size_t n, char c);

    // ---------- Удаление ----------
    String& erase(std::size_t pos = 0, std::size_t len = npos);

    // ---------- Замена ----------
    String& replace(std::size_t pos, std::size_t len, const String& str);
    String& replace(std::size_t pos, std::size_t len, const char* str);
    String& replace(std::size_t pos, std::size_t len, std::size_t n, char c);

    // ---------- Обмен / данные ----------
    void        swap(String& str);
    const char* data() const;

    // ---------- Поиск ----------
    std::size_t find(const String& str, std::size_t pos = 0) const;
    std::size_t find(const char* str, std::size_t pos = 0) const;
    std::size_t find(char c, std::size_t pos = 0) const;

    // ---------- Подстрока / сравнение ----------
    String substr(std::size_t pos = 0, std::size_t len = npos) const;

    int compare(const String& str) const;
    int compare(const char* str) const;

    // ---------- Константа ----------
    static const std::size_t npos = static_cast<std::size_t>(-1);

    // ---------- Для unit-тестов ----------
    std::size_t countRef() const;

private:
    // Control block: определение скрыто в .cpp
    struct StringData;
    StringData* shared_ = nullptr;

    // ---------- Управление control block ----------
    // Создаёт блок с буфером ёмкости cap, копирует n символов из src
    // и ставит '\0' в позицию n. Может бросить std::bad_alloc.
    static StringData* acquire(const char* src, std::size_t n, std::size_t cap);

    static void add_ref(StringData* p);
    static void release(StringData* p);   // уменьшает счётчик, при 0 — освобождает

    // ---------- Внутренние хелперы ----------
    void detach();                        // COW: копирует буфер, если он разделяемый
    void ensure_capacity(std::size_t n);       // расширяет буфер до n (не уменьшает)

    // Общий бэкенд для перегрузок insert/replace/+= — чтобы не дублировать логику
    String& append_raw(const char* src, std::size_t n);
    String& insert_raw(std::size_t pos, const char* src, std::size_t n);
    String& replace_raw(std::size_t pos, std::size_t len, const char* src, std::size_t n);

    // Общий поиск подстроки (используется find(const String&) и find(const char*))
    std::size_t find_raw(const char* needle, std::size_t n, std::size_t pos) const;
    static const char& null_char();
    String& append_raw(const char* src, std::size_t n);
    String& insert_raw(std::size_t pos, const char* src, std::size_t n);
    String& String::replace_raw(std::size_t pos, std::size_t len,
                            const char* src, std::size_t n);
    std::size_t find_raw(const char* needle, std::size_t n,
                     std::size_t pos) const;
};
