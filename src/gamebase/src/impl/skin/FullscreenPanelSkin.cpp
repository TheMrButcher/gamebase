#include <stdafx.h>
#include <gamebase/impl/skin/impl/FullscreenPanelSkin.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

void FullscreenPanelSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    m_background.registerProperties("", builder);
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

} }
