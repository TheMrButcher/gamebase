/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/gameobj/PressableElement.h>
#include <gamebase/impl/gameobj/ClickableElement.h>
#include <gamebase/impl/gameobj/PressableAndClickableElement.h>

namespace gamebase { namespace editor {

class CommonSelectableElement : public impl::ClickableElement {
public:
    CommonSelectableElement();
    CommonSelectableElement(bool isPressable, Time clickTime);
    virtual void serialize(impl::Serializer&) const override;

private:
    bool m_isPressable;
};

std::unique_ptr<impl::IObject> deserializePressableElement(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializeClickableElement(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializePressableAndClickableElement(impl::Deserializer& deserializer);

} }
