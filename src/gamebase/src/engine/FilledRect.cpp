#include <stdafx.h>
#include <gamebase/engine/FilledRect.h>

namespace gamebase {

void FilledRect::loadResources()
{
    TextureRect::loadResources();
    setTexture(loadTexture(WHITE_RECT_IMAGE_ID, &whiteRectImage));
}

}
