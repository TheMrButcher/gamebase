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

class MoveAction : virtual public IAction, public ISerializable {
public:
    MoveAction() {}

    MoveAction(const std::string& dstLayerName)
        : m_dstLayerName(dstLayerName)
    {}

    virtual void load(const PropertiesRegister& props) override;

    virtual void exec() override;

    virtual void serialize(Serializer& s) const override;

private:
    boost::optional<std::string> m_dstLayerName;
    IRegistrable* m_obj;
};

} }
