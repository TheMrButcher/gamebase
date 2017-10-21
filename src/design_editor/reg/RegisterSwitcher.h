/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace editor {

class RegisterSwitcher {
public:
    enum RegType {
        ResultReg,
        DVBReg
    };

    RegisterSwitcher();
    ~RegisterSwitcher();
    static void init();
    static RegType current();

private:
    RegType startingReg_;
};

} }
