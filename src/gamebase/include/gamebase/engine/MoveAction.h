#pragma once

#include <gamebase/engine/IAction.h>
#include <gamebase/engine/PropertiesRegister.h>
#include <gamebase/engine/ILayer.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class MoveAction : virtual public IAction, public ISerializable {
public:
    MoveAction(int dstLayerID)
        : m_dstLayerID(dstLayerID)
    {}

    virtual void load(const PropertiesRegister& props) override;

    virtual void exec() override;

    virtual void serialize(Serializer& s) const override;

private:
    int m_dstLayerID;
    IRegistrable* m_obj;
};

}
