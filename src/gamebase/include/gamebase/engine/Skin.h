#pragma once

#include <gamebase/engine/Selectable.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>

namespace gamebase {

class Skin : public virtual Selectable, public virtual Drawable, public virtual Registrable {
};

}
