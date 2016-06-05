#pragma once

#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleApplication : public Application {
public:
    SimpleApplication();

    virtual void initView() override;
};

} }
