#include "Buffer.hpp"
#include <string>

void Buffer::append(const char* _str, int _size) {
    for (int i = 0; i < _size; i++) {
        if (_str[i] == '\0') {
            break;
        }
        buf_.push_back(_str[i]);
    }
}

size_t Buffer::getSize() const {
    return buf_.size();
}

const char* Buffer::c_str() const {
    return buf_.c_str();
}

void Buffer::clear() {
    buf_.clear();
}

void Buffer::setBuf(const char* str) {
    buf_.clear();
    buf_.append(str);
}