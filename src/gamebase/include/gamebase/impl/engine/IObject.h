/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace impl {

class IObject {
public:
    IObject() {}

    virtual ~IObject() {}

private:
    IObject(const IObject&) {}
    IObject(IObject&&) {}
    IObject& operator=(const IObject&) { return *this; }
    IObject& operator=(IObject&&) { return *this; }
};

} }
