#pragma once

#include <string>
#include "common.hpp"

class Buffer {
    std::string buf_;
public:
    DISALLOW_COPY_AND_MOVE(Buffer);
    Buffer() = default;
    ~Buffer() = default;

    void append(const char* _str, int _size);
    size_t getSize() const;
    const char* c_str() const;
    void clear();
    void setBuf(const char*);
};
