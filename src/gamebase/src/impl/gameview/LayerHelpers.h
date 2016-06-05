#pragma once

#include <gamebase/impl/gameview/ImmobileLayer.h>

namespace gamebase { namespace impl {

class Deserializer;

void deserializeLayerContents(Deserializer& deserializer, ImmobileLayer* layer);

} }
