/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/RegisterSnapshot.h>

namespace gamebase { namespace editor {

class ResultBuilderRegisterSnapshot : public RegisterSnapshot {
public:
    static const RegisterSnapshot& instance()
    {
        static ResultBuilderRegisterSnapshot SNAPSHOT;
        return SNAPSHOT;
    }

private:
    ResultBuilderRegisterSnapshot();
};

} }
