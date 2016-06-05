#pragma once

#include <string>
#include <vector>

namespace gamebase { namespace impl {

class Utf8Text {
public:
    Utf8Text() {}

    Utf8Text(const std::string& text);

    const std::string& toString() const { return m_text; }

    void set(const std::string& text);
    void insert(size_t pos, const std::string& str);
    void erase(size_t pos);
    void erase(size_t posBegin, size_t posEnd);
    Utf8Text substr(size_t pos, size_t count = size_t(-1)) const;

    void clear()
    {
        m_utf8Chars.clear();
        m_text.clear();
    }

    size_t size() const { return m_utf8Chars.size(); }
    bool empty() const { return m_utf8Chars.empty(); }

private:
    std::vector<std::string> m_utf8Chars;
    std::string m_text;
};

} }
