/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/RegisterSnapshot.h>

namespace gamebase { namespace editor {

class DesignViewBuilderRegisterSnapshot : public RegisterSnapshot {
public:
    static const RegisterSnapshot& instance()
    {
        static DesignViewBuilderRegisterSnapshot SNAPSHOT;
        return SNAPSHOT;
    }

private:
    DesignViewBuilderRegisterSnapshot();
};

} }
