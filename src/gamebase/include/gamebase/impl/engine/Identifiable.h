#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

class Identifiable : virtual public IObject {
public:
    static const int INVALID_ID = 0;

    Identifiable(int id = INVALID_ID)
        : m_id(m_id)
    {}

    int id() const { return m_id; }
    bool hasValidID() const { return m_id != INVALID_ID; }
    void setID(int id) { m_id = id; }

    static int generateID(IObject* obj, int suggestedID)
    {
        if (auto* identifiable = dynamic_cast<Identifiable*>(obj)) {
            if (identifiable->hasValidID())
                return identifiable->id();
            identifiable->setID(suggestedID);
        }
        return suggestedID;
    }

private:
    int m_id;
};

} }
