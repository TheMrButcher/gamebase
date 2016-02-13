#pragma once

#include <gamebase/engine/IAction.h>
#include <gamebase/engine/PropertiesRegister.h>
#include <gamebase/engine/ILayer.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class RemoveAction : virtual public IAction, public ISerializable {
public:
    virtual void load(const PropertiesRegister& props) override;

    virtual void exec() override;

    virtual void serialize(Serializer& s) const override;

private:
    IRegistrable* m_obj;
};

}
