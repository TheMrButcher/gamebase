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
