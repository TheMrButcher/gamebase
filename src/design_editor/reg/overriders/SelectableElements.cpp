/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SelectableElements.h"

namespace gamebase { namespace editor {

CommonSelectableElement::CommonSelectableElement()
    : m_isPressable(false)
{
    m_clickTime = 0;
}

CommonSelectableElement::CommonSelectableElement(bool isPressable, Time clickTime)
    : m_isPressable(isPressable)
{
    m_clickTime = clickTime;
}

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

std::unique_ptr<impl::IObject> deserializePressableElement(impl::Deserializer& deserializer)
{
    return std::make_unique<CommonSelectableElement>(true, 0);
}

std::unique_ptr<impl::IObject> deserializeClickableElement(impl::Deserializer& deserializer)
{
    DESERIALIZE(Time, clickTime);
    return std::make_unique<CommonSelectableElement>(false, clickTime);
}

std::unique_ptr<impl::IObject> deserializePressableAndClickableElement(impl::Deserializer& deserializer)
{
    DESERIALIZE(Time, clickTime);
    return std::make_unique<CommonSelectableElement>(true, clickTime);
}

} }
