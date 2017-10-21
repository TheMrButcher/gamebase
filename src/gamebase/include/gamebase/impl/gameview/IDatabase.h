/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace impl {

class IDatabase {
public:
    IDatabase() {}
    virtual ~IDatabase() {}

    virtual void remove(int id) = 0;
    virtual void clear() = 0;

private:
    IDatabase(const IDatabase&) {}
    IDatabase(IDatabase&&) {}
    IDatabase& operator=(const IDatabase&) { return *this; }
    IDatabase& operator=(IDatabase&&) { return *this; }
};

} }
