// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
#pragma once

// Minimal header-only UTF-8 decoder.
// Decodes a UTF-8 string view into a sequence of Unicode codepoints (char32_t).
//
// Usage:
//   for (char32_t cp : Utf8View("hello θ π")) { ... }

#include <string_view>
#include <cstdint>

struct Utf8View {
    const char* ptr;
    const char* end;

    explicit Utf8View(std::string_view sv)
        : ptr(sv.data()), end(sv.data() + sv.size()) {}

    struct Iterator {
        const char* p;
        const char* e;
        char32_t    current;

        bool operator!=(const Iterator& o) const { return p != o.p; }
        Iterator& operator++() { advance(); return *this; }
        char32_t  operator*() const { return current; }

        void advance() {
            if (p >= e) { p = e; return; }
            unsigned char c = static_cast<unsigned char>(*p);
            if (c < 0x80) {
                current = c; ++p;
            } else if ((c & 0xE0) == 0xC0) {
                current = (c & 0x1F);
                if (p + 1 < e) current = (current << 6) | (static_cast<unsigned char>(p[1]) & 0x3F);
                p += 2;
            } else if ((c & 0xF0) == 0xE0) {
                current = (c & 0x0F);
                if (p + 1 < e) current = (current << 6) | (static_cast<unsigned char>(p[1]) & 0x3F);
                if (p + 2 < e) current = (current << 6) | (static_cast<unsigned char>(p[2]) & 0x3F);
                p += 3;
            } else {
                current = (c & 0x07);
                for (int i = 1; i < 4 && p + i < e; ++i)
                    current = (current << 6) | (static_cast<unsigned char>(p[i]) & 0x3F);
                p += 4;
            }
        }
    };

    Iterator begin() const {
        Iterator it{ptr, end, 0};
        it.advance();
        return it;
    }
    Iterator end_it() const { return Iterator{end, end, 0}; }

    // Range-for compatibility
    Iterator begin_range() const { return begin(); }
    Iterator end_range()   const { return end_it(); }
};

// Range-for support via free functions
inline Utf8View::Iterator begin(const Utf8View& v) { return v.begin(); }
inline Utf8View::Iterator end(const Utf8View& v)   { return v.end_it(); }
