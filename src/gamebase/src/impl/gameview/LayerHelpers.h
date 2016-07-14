/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/ImmobileLayer.h>

namespace gamebase { namespace impl {

class Deserializer;

void deserializeLayerContents(Deserializer& deserializer, ImmobileLayer* layer);

} }
