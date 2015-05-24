#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/IRelativeOffset.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API OffsettedBox : public IRelativeBox, public ISerializable {
public:
    OffsettedBox(
        const std::shared_ptr<IRelativeOffset>& offset = nullptr)
        : m_offset(offset)
    {}

    virtual BoundingBox count(const BoundingBox& parentBox) const override
    {
        auto result = parentBox;
        if (m_offset)
            result.move(m_offset->count(parentBox, parentBox));
        return result;
    }

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeOffset> m_offset;
};

}
