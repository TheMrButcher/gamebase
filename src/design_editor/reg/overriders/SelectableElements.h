/**
 * Copyright (c) 2016 Slavnejshev Filipp
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
    virtual void serialize(impl::Serializer&) const override;

private:
    bool m_isPressable;
};

void serializePressableElement(const impl::IObject* obj, impl::Serializer& s);
void serializeClickableElement(const impl::IObject* obj, impl::Serializer& s);
void serializePressableAndClickableElement(const impl::IObject* obj, impl::Serializer& s);

std::unique_ptr<impl::IObject> deserializeCommonSelectableElement(impl::Deserializer& deserializer);

} }
