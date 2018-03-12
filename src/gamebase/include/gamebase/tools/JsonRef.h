/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <vector>

namespace gamebase {

class JsonImpl;
class Json;

class GAMEBASE_API JsonRef {
public:
    bool toBool() const;
    int64_t toInt() const;
    double toFloat() const;
    std::string toString() const;

    bool isBool() const;
    bool isInt() const;
    bool isFloat() const;
    bool isString() const;
    bool isObject() const;
    bool isArray() const;

    int size() const;
    bool empty() const;
    bool isEmpty() const { return empty(); }
    void clear();

    bool has(const std::string& name) const;
    void remove(const std::string& name);
    std::vector<std::string> members() const;
    void add(const Json& value);

    JsonRef get(int index);
    JsonRef get(const std::string& name);

    JsonRef operator[](int index);
    JsonRef operator[](const std::string& name);

public:
    JsonRef();
    JsonRef(const JsonRef& other);
    JsonRef(JsonRef&& other);

    JsonRef& operator=(const JsonRef& other);
    JsonRef& operator=(JsonRef&& other);
    JsonRef& operator=(const Json& other);
    JsonRef& operator=(Json&& other);
    JsonRef get(const char* name);
    JsonRef operator[](const char* name);
    void add(const JsonRef& value);
    bool operator==(const JsonRef& other) const;
    bool operator!=(const JsonRef& other) const;
    bool operator==(const Json& other) const;
    bool operator!=(const Json& other) const;

private:
    JsonRef(const std::shared_ptr<JsonImpl>& impl);
    friend class Json;
    std::shared_ptr<JsonImpl> m_impl;
};

}
