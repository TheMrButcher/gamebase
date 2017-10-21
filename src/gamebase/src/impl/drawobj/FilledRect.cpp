/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/FilledRect.h>

namespace gamebase { namespace impl {

void FilledRect::loadResources()
{
    TextureRect::loadResources();
    setTexture(loadTexture(WHITE_RECT_IMAGE_ID, &whiteRectImage));
}

} }
