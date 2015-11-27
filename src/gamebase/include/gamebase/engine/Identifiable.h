#pragma once

#include <gamebase/engine/IObject.h>

namespace gamebase {

class Identifiable : virtual public IObject {
public:
    static const int INVALID_ID = -1;

    Identifiable(int id = INVALID_ID)
        : m_id(m_id)
    {}

    int id() const { return m_id; }
    bool hasValidID() const { return m_id != INVALID_ID; }
    void setID(int id) { m_id = id; }

private:
    int m_id;
};

}
