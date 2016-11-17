/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Properties.h>

namespace gamebase { namespace editor {
struct MapProperties {
    MapProperties();

    struct Element {
        Element();
        Element(const std::shared_ptr<Properties>& properties, int keyNodeID);

        std::shared_ptr<Properties> properties;
        int keyNodeID;
    };
    std::vector<Element> elements;
    size_t currentElem;
    int keysArrayNodeID;
};
} }
