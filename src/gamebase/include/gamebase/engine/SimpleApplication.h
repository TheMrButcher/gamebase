#pragma once

#include <gamebase/engine/Application.h>

namespace gamebase {

class GAMEBASE_API SimpleApplication : public Application {
public:
    SimpleApplication();

    virtual void initView() override;
};

}
