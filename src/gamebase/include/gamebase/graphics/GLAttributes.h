#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/typedefs.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API GLAttributes {
public:
    enum AttrID {
        Position = 0,
        Distance = 1,
        ExtVec = 2
    };

    GLAttributes(): m_size(0) {}

    GLAttributes& add(AttrID id, size_t size)
    {
        m_attrs.push_back(Attribute(id, m_size, size));
        m_size += size;
        return *this;
    }

    void activate() const;
    void disable() const;

private:
    size_t m_size;
    
    struct Attribute {
        Attribute(
            GLuint id,
            size_t offset,
            size_t size)
            : id(id)
            , offset(offset)
            , size(size)
        {}

        GLuint id;
        size_t offset;
        size_t size;
    };
    std::vector<Attribute> m_attrs;
};

}
