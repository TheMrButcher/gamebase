#include <stdafx.h>
#include <gamebase/engine/Layer.h>
#include "LayerHelpers.h"
#include <gamebase/serial/IDeserializer.h>
#include "src/core/GlobalTemporary.h"

namespace gamebase {

Layer::Layer()
    : ImmobileLayer()
{}
    
void Layer::drawAt(const Transform2& position) const
{
    ImmobileLayer::drawAt(position);
    g_temp.delayedTasks.push_back(std::bind(
        &Layer::setNeedUpdate, this));
}

std::unique_ptr<IObject> deserializeLayer(Deserializer& deserializer)
{
    std::unique_ptr<Layer> result(new Layer());
    deserializeLayerContents(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(Layer);

}
