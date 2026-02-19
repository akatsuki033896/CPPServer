#include "Buffer.hpp"
#include <string>
#include <iostream>

void Buffer::append(const char* _str, int _size) {
    for (int i = 0; i < _size; i++) {
        if (_str[i] == '\0') {
            break;
        }
        buf_.push_back(_str[i]);
    }
}

size_t Buffer::size() const {
    return buf_.size();
}

const char* Buffer::c_str() const {
    return buf_.c_str();
}

void Buffer::clear() {
    buf_.clear();
}

void Buffer::getline() {
    buf_.clear();
    std::getline(std::cin, buf_);
}

void Buffer::setBuf(const char* str) {
    buf_.clear();
    buf_.append(str);
}