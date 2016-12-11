/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SelectableElements.h"

namespace gamebase { namespace editor {

void CommonSelectableElement::serialize(impl::Serializer& s) const
{
    s << "isPressable" << m_isPressable;
    impl::ClickableElement::serialize(s);
}

std::unique_ptr<impl::IObject> deserializeCommonSelectableElement(impl::Deserializer& deserializer)
{
    DESERIALIZE(bool, isPressable);
    DESERIALIZE(Time, clickTime);
    if (isPressable) {
        if (clickTime > 0) {
            std::unique_ptr<impl::PressableAndClickableElement> result(
                new impl::PressableAndClickableElement());
            result->setClickTime(clickTime);
            return std::move(result);
        } else {
            return std::unique_ptr<impl::IObject>(new impl::PressableElement());
        }
    } else {
        if (clickTime > 0) {
            std::unique_ptr<impl::ClickableElement> result(new impl::ClickableElement());
            result->setClickTime(clickTime);
            return std::move(result);
        }
    }
    return std::unique_ptr<impl::IObject>();
}

REGISTER_CLASS(CommonSelectableElement);

void serializePressableElement(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto selectableElem = dynamic_cast<const impl::PressableElement*>(obj)) {
        s   << "isPressable" << true
            << "clickTime" << 0;
    } else {
        THROW_EX() << "Serializer expected PressableElement as input";
    }
}

void serializeClickableElement(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto selectableElem = dynamic_cast<const impl::ClickableElement*>(obj)) {
        s   << "isPressable" << false
            << "clickTime" << selectableElem->clickTime();
    } else {
        THROW_EX() << "Serializer expected ClickableElement as input";
    }
}

void serializePressableAndClickableElement(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto selectableElem = dynamic_cast<const impl::PressableAndClickableElement*>(obj)) {
        s   << "isPressable" << true
            << "clickTime" << selectableElem->clickTime();
    } else {
        THROW_EX() << "Serializer expected PressableAndClickableElement as input";
    }
}

} }
