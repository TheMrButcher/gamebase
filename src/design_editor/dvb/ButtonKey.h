/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace editor {
struct ButtonKey {
    enum Enum {
        Add,
        AddFromFile,
        Remove,
        Save,
        ReplaceFromFile,
        Down,
        Up,
        Cut,
        Copy,
        Paste,
        AddFromClipboard
    };
};
} }
