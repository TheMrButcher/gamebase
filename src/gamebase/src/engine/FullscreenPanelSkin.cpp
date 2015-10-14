#include <stdafx.h>
#include <gamebase/engine/FullscreenPanelSkin.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

void FullscreenPanelSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    m_background.registerProperties("background", builder);
}

void FullscreenPanelSkin::serialize(Serializer& s) const
{
    s << "backgroundColor" << backgroundColor();
}

std::unique_ptr<IObject> deserializeFullscreenPanelSkin(Deserializer& deserializer)
{
    DESERIALIZE(Color, backgroundColor);
    return std::unique_ptr<FullscreenPanelSkin>(new FullscreenPanelSkin(backgroundColor));
}

REGISTER_CLASS(FullscreenPanelSkin);

}
