/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/relbox/SquareBox.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/relbox/ComplexBox.h>

namespace gamebase { namespace editor {

void serializeRelativeBoxAsComplex(const impl::IObject* obj, impl::Serializer& s);
void serializeOffsettedBoxAsComplex(const impl::IObject* obj, impl::Serializer& s);
void serializeSquareBoxAsComplex(const impl::IObject* obj, impl::Serializer& s);
void serializeFixedBoxAsComplex(const impl::IObject* obj, impl::Serializer& s);
void serializePixelBoxAsComplex(const impl::IObject* obj, impl::Serializer& s);

std::unique_ptr<impl::IObject> deserializeBox(impl::Deserializer& deserializer);

} }
