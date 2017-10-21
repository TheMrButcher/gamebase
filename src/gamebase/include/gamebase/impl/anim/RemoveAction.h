/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/anim/IAction.h>
#include <gamebase/impl/reg/PropertiesRegister.h>
#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class RemoveAction : virtual public IAction, public ISerializable {
public:
    virtual void load(const PropertiesRegister& props) override;

    virtual void exec() override;

    virtual void serialize(Serializer& s) const override;

private:
    IRegistrable* m_obj;
};

} }
