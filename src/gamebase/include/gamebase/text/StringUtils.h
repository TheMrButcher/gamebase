/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>

namespace gamebase {

template <typename T>
std::string toString(const T& t)
{
    return boost::lexical_cast<std::string>(t);
}

template <typename T>
std::string toString(const T& t, size_t maxCharsNum)
{
    std::ostringstream ss;
    ss.precision(maxCharsNum >= 2 ? maxCharsNum - 2 : 0);
    ss << std::fixed << t;
    auto result = ss.str();
    if (result.length() > maxCharsNum)
        result.resize(maxCharsNum);
    return result;
}

inline float toFloat(const std::string& str)
{
    return boost::lexical_cast<float>(str);
}

inline int toInt(const std::string& str)
{
    return boost::lexical_cast<int>(str);
}

inline double toDouble(const std::string& str)
{
    return boost::lexical_cast<double>(str);
}

inline std::string addSlash(const std::string& path)
{
    if (path.empty() || path.back() == '\\')
        return path;
    return path + '\\';
}

}
