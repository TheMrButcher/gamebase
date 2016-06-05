#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/graphics/typedefs.h>
#include <vector>

namespace gamebase { namespace impl {

class GAMEBASE_API GLAttributes {
public:
    typedef std::string AttrID;

    GLAttributes(): m_size(0) {}

    GLAttributes& add(const AttrID& id, size_t size)
    {
        m_attrs.push_back(Attribute(id, m_size, size));
        m_size += size;
        return *this;
    }

    void locate(int programID, const std::string& programName);
    void activate() const;
    void disable() const;

private:
    size_t m_size;
    
    struct Attribute {
        Attribute(
            const AttrID& attrID,
            size_t offset,
            size_t size)
            : name(attrID)
            , id(0)
            , offset(offset)
            , size(size)
        {}

        AttrID name;
        GLuint id;
        size_t offset;
        size_t size;
    };
    std::vector<Attribute> m_attrs;
};

} }
