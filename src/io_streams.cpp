#include "io_streams.h"

#include <fcntl.h>
#include <unistd.h>

namespace ra {

bool OutputStream::writeChar(char value) {
    return writeBytes(&value, 1);
}

bool OutputStream::writeText(const char* text) {
    if (text == nullptr) {
        return false;
    }

    std::size_t length = 0;
    while (text[length] != '\0') {
        ++length;
    }

    return writeBytes(text, length);
}

bool OutputStream::writeString(const std::string& text) {
    return writeBytes(text.c_str(), text.size());
}

bool ConsoleOutputStream::writeBytes(const char* data, std::size_t size) {
    std::size_t total_written = 0;
    while (total_written < size) {
        const ssize_t current = ::write(STDOUT_FILENO, data + total_written, size - total_written);
        if (current <= 0) {
            return false;
        }

        total_written += static_cast<std::size_t>(current);
    }

    return true;
}

FileInputStream::FileInputStream()
    : descriptor_(-1), buffer_size_(0), buffer_position_(0), reached_end_(false) {}

FileInputStream::FileInputStream(const std::string& path)
    : descriptor_(-1), path_(path), buffer_size_(0), buffer_position_(0), reached_end_(false) {}

FileInputStream::~FileInputStream() {
    close();
}

void FileInputStream::setPath(const std::string& path) {
    path_ = path;
}

bool FileInputStream::open() {
    close();

    descriptor_ = ::open(path_.c_str(), O_RDONLY);
    buffer_size_ = 0;
    buffer_position_ = 0;
    reached_end_ = descriptor_ < 0;
    return descriptor_ >= 0;
}

bool FileInputStream::isOpen() const {
    return descriptor_ >= 0;
}

bool FileInputStream::readLine(std::string& line) {
    line.clear();

    while (true) {
        if (buffer_position_ >= buffer_size_) {
            if (!fillBuffer()) {
                return !line.empty();
            }
        }

        while (buffer_position_ < buffer_size_) {
            const char current = buffer_[buffer_position_];
            ++buffer_position_;

            if (current == '\r') {
                continue;
            }

            if (current == '\n') {
                return true;
            }

            line += current;
        }
    }
}

void FileInputStream::close() {
    if (descriptor_ >= 0) {
        ::close(descriptor_);
        descriptor_ = -1;
    }

    buffer_size_ = 0;
    buffer_position_ = 0;
    reached_end_ = false;
}

bool FileInputStream::fillBuffer() {
    if (descriptor_ < 0 || reached_end_) {
        return false;
    }

    const ssize_t bytes_read = ::read(descriptor_, buffer_, sizeof(buffer_));
    if (bytes_read <= 0) {
        reached_end_ = true;
        return false;
    }

    buffer_size_ = static_cast<std::size_t>(bytes_read);
    buffer_position_ = 0;
    return true;
}

ConsoleOutputStream& console() {
    static ConsoleOutputStream stream;
    return stream;
}

}  // namespace ra
