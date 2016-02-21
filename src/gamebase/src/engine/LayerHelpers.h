#pragma once

#include <gamebase/engine/ImmobileLayer.h>

namespace gamebase {

class Deserializer;

void deserializeLayerContents(Deserializer& deserializer, ImmobileLayer* layer);

}
