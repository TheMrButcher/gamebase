/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/reg/IRegistrable.h>
#include <vector>

namespace gamebase { namespace impl {

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

} }
