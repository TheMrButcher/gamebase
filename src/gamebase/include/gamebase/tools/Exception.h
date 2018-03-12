/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <exception>
#include <string>
#include <sstream>

namespace gamebase {

namespace details {

class ExceptionMessageBuilder {
public:
    ExceptionMessageBuilder() {}
    ExceptionMessageBuilder(const std::string& message): m_message(message) {}

    const std::string& message() const { return m_message; }

    template <class T>
    ExceptionMessageBuilder operator<<(const T& t) const
    {
        std::ostringstream ss;
        ss << m_message << t;
        return ExceptionMessageBuilder(ss.str());
    }

private:
    std::string m_message;
};

}

class Exception : public std::exception {
public:
    Exception() {}
    Exception(const std::string& message): std::exception(message.c_str()) {}

    Exception operator|(const details::ExceptionMessageBuilder& messageBuilder) const
    {
        return Exception(messageBuilder.message());
    }
};

#define THROW_EX() throw Exception() | details::ExceptionMessageBuilder()

}
