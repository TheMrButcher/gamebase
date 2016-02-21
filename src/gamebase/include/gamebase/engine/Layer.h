#pragma once

#include <gamebase/engine/ImmobileLayer.h>

namespace gamebase {

class GAMEBASE_API Layer : public ImmobileLayer {
public:
    Layer();
    
    virtual void drawAt(const Transform2& position) const override;
};

}
