#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/PropertiesRegister.h>
#include <string>

namespace gamebase {

class PropertiesRegisterBuilder;

class IRegistrable : public virtual IObject {
public:
    virtual void setName(const std::string& name) = 0;
    
    virtual std::string name() const = 0;

    virtual std::string fullName() const = 0;

    virtual void registerObject(PropertiesRegisterBuilder* builder) = 0;

    virtual PropertiesRegister& properties() = 0;
};

}
