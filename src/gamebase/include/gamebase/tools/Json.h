/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/tools/JsonRef.h>

namespace gamebase {

enum JsonFormat {
    FastJsonFormat,
    StyledJsonFormat
};

GAMEBASE_API Json loadJsonFromFile(const std::string& fileName);
GAMEBASE_API Json loadJsonFromString(const std::string& text);
GAMEBASE_API void saveJsonToFile(const std::string& fileName, const Json& json, JsonFormat format = FastJsonFormat);
GAMEBASE_API std::string toString(const Json& json, JsonFormat format = FastJsonFormat);

class GAMEBASE_API Json {
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

    void load(const std::string& fileName);
    void save(const std::string& fileName, JsonFormat format = FastJsonFormat) const;

    void parse(const std::string& text);
    std::string serialize(JsonFormat format = FastJsonFormat) const;

// IMPLEMENTATION DETAILS

public:
    Json();
    Json(bool value);
    Json(int value);
    Json(int64_t value);
    Json(unsigned int value);
    Json(uint64_t value);
    Json(float value);
    Json(double value);
    Json(const char* value);
    Json(const std::string& value);
    Json(const Json& other);
    Json(Json&& other);
    Json(const JsonRef& other);
    Json(JsonRef&& other);

    Json& operator=(const Json& other);
    Json& operator=(Json&& other);
    Json& operator=(const JsonRef& other);
    Json& operator=(JsonRef&& other);
    JsonRef get(const char* name);
    JsonRef operator[](const char* name);
    void add(const JsonRef& value);
    bool operator==(const Json& other) const;
    bool operator!=(const Json& other) const;
    bool operator==(const JsonRef& other) const;
    bool operator!=(const JsonRef& other) const;

private:
    Json(const std::shared_ptr<JsonImpl>& impl);
    friend class JsonRef;
    std::shared_ptr<JsonImpl> m_impl;
};

}
