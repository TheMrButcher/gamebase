#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/engine/PressableButton.h>
#include <gamebase/engine/TextEdit.h>
#include <gamebase/engine/ButtonList.h>

namespace gamebase {

class TextListSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<PressableButton> createOpenButton() const = 0;

    virtual std::shared_ptr<TextEdit> createTextEdit() const = 0;

    virtual std::shared_ptr<ButtonList> createList() const = 0;

    virtual BoundingBox listBox() const = 0;
};

}
