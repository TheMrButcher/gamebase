#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relpos/IRelativeOffset.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

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

} }
