#pragma once

#include <gamebase/engine/IRegistrable.h>
#include <vector>

namespace gamebase {

class Registrable : public virtual IRegistrable {
public:
    virtual void setName(const std::string& name) override
    {
        m_register.setName(name);
    }
    
    virtual std::string name() const override
    {
        return m_register.name();
    }

    virtual std::string fullName() const override
    {
        return m_register.fullName();
    }

    virtual PropertiesRegister& properties() override { return m_register; }

protected:
    PropertiesRegister m_register;
};

template <typename Base>
class Reg : public Base, public Registrable {
public:
    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        Base::registerProperties("", builder);
    }
};

}
