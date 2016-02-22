#pragma once

namespace gamebase {

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

}
