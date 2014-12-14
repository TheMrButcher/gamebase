#pragma once

#include <string>

namespace gamebase {

class ITextFilter {
public:
    virtual ~ITextFilter() {}

    virtual std::string filter(const std::string& text) const = 0;
};

}
