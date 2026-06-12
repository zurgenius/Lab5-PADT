#pragma once

#include <cstddef>
#include <string>

namespace ra {

class OutputStream {
public:
    virtual ~OutputStream() = default;

    virtual bool writeBytes(const char* data, std::size_t size) = 0;

    bool writeChar(char value);
    bool writeText(const char* text);
    bool writeString(const std::string& text);
};

class ConsoleOutputStream : public OutputStream {
public:
    bool writeBytes(const char* data, std::size_t size) override;
};

class FileInputStream {
public:
    FileInputStream();
    explicit FileInputStream(const std::string& path);
    ~FileInputStream();

    void setPath(const std::string& path);
    bool open();
    bool isOpen() const;
    bool readLine(std::string& line);
    void close();

private:
    bool fillBuffer();

    int descriptor_;
    std::string path_;
    char buffer_[4096];
    std::size_t buffer_size_;
    std::size_t buffer_position_;
    bool reached_end_;
};

ConsoleOutputStream& console();

}  // namespace ra
