#pragma once

#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/serial/SerializableRegister.h>
#include <gamebase/impl/serial/constants.h>
#include <gamebase/impl/engine/RelativeValue.h>
#include <gamebase/impl/app/TimeState.h>
#include <gamebase/impl/reg/IRegistrable.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/text/AlignProperties.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/graphics/Color.h>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace gamebase { namespace impl {

class ISerializer {
public:
    virtual ~ISerializer() {}

    virtual void writeFloat(const std::string& name, float f) = 0;

    virtual void writeDouble(const std::string& name, double d) = 0;

    virtual void writeInt(const std::string& name, int i) = 0;
    
    virtual void writeUInt(const std::string& name, unsigned int i) = 0;

    virtual void writeInt64(const std::string& name, int64_t i) = 0;

    virtual void writeUInt64(const std::string& name, uint64_t i) = 0;

    virtual void writeBool(const std::string& name, bool b) = 0;

    virtual void writeString(const std::string& name, const std::string& value) = 0;

    virtual void startObject(const std::string& name) = 0;

    virtual void finishObject() = 0;

    virtual void startArray(const std::string& name, SerializationTag::Type tag) = 0;

    virtual void finishArray() = 0;
};

namespace internal {
template <typename Iter>
typename Iter::reference deref(Iter it)
{
    return *it;
}

template <typename T, typename Iter>
const T& derefKey(Iter it)
{
    return it->first;
}

template <typename T, typename Iter>
const T& derefValue(Iter it)
{
    return it->second;
}
}

class Serializer {
public:
    Serializer(ISerializer* serializer)
        : m_serializer(serializer)
    {}

    class ValueSerializer {
    public:
        ValueSerializer(ISerializer* serializer, const std::string& name)
            : m_serializer(serializer)
            , m_name(name)
        {}

        Serializer operator<<(float f) const
        {
            m_serializer->writeFloat(m_name, f);
            return Serializer(m_serializer);
        }

        Serializer operator<<(double d) const
        {
            m_serializer->writeDouble(m_name, d);
            return Serializer(m_serializer);
        }

        Serializer operator<<(int i) const
        {
            m_serializer->writeInt(m_name, i);
            return Serializer(m_serializer);
        }

        Serializer operator<<(unsigned int i) const
        {
            m_serializer->writeUInt(m_name, i);
            return Serializer(m_serializer);
        }

        Serializer operator<<(int64_t i) const
        {
            m_serializer->writeInt64(m_name, i);
            return Serializer(m_serializer);
        }

        Serializer operator<<(uint64_t i) const
        {
            m_serializer->writeUInt64(m_name, i);
            return Serializer(m_serializer);
        }
        
        Serializer operator<<(bool b) const
        {
            m_serializer->writeBool(m_name, b);
            return Serializer(m_serializer);
        }

        template <typename EnumType>
        typename std::enable_if<std::is_enum<EnumType>::value, Serializer>::type operator<<(
            EnumType enumValue) const
        {
            return operator<<(static_cast<int>(enumValue));
        }

        Serializer operator<<(const std::string& s) const
        {
            m_serializer->writeString(m_name, s);
            return Serializer(m_serializer);
        }

        Serializer operator<<(const Vec2& v) const
        {
            m_serializer->startArray(m_name, SerializationTag::Vec2);
            writeVector(v);
            m_serializer->finishArray();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const Matrix2& m) const
        {
            m_serializer->startArray(m_name, SerializationTag::Matrix2);
            writeMatrix(m);
            m_serializer->finishArray();
            return Serializer(m_serializer);
        }
        
        Serializer operator<<(const Transform2& t) const
        {
            m_serializer->startArray(m_name, SerializationTag::Transform2);
            writeMatrix(t.matrix);
            writeVector(t.offset);
            m_serializer->finishArray();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const BoundingBox& b) const
        {
            m_serializer->startArray(m_name, SerializationTag::BoundingBox);
            writeVector(b.bottomLeft);
            writeVector(b.topRight);
            m_serializer->finishArray();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const Color& c) const
        {
            m_serializer->startArray(m_name, SerializationTag::Color);
            m_serializer->writeFloat("", c.r);
            m_serializer->writeFloat("", c.g);
            m_serializer->writeFloat("", c.b);
            m_serializer->writeFloat("", c.a);
            m_serializer->finishArray();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const RelativeValue& relVal) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "RelativeValue");
            Serializer relValSerializer(m_serializer);
            relValSerializer << "type" << relVal.type();
            relValSerializer << "value" << relVal.value();
            m_serializer->finishObject();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const TypedTime& time) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "TypedTime");
            Serializer timeSerializer(m_serializer);
            timeSerializer << "type" << time.type << "value" << time.value;
            m_serializer->finishObject();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const FontDesc& fontDesc) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "FontDesc");
            Serializer fontDescSerializer(m_serializer);
            fontDescSerializer << "family" << fontDesc.fontFamily << "size" << fontDesc.size;
            m_serializer->finishObject();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const AlignProperties& props) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "AlignProperties");
            Serializer propsSerializer(m_serializer);
            propsSerializer << "font" << props.font << "horAlign" << props.horAlign
                << "vertAlign" << props.vertAlign << "enableStacking" << props.enableStacking;
            m_serializer->finishObject();
            return Serializer(m_serializer);
        }

        Serializer operator<<(const IObject& obj) const
        {
            try {
                m_serializer->startObject(m_name);
                std::string typeName = SerializableRegister::instance().typeName(typeid(obj));
                m_serializer->writeString(TYPE_NAME_TAG, typeName);
                m_serializer->writeBool(EMPTY_TAG, false);
                Serializer objectSerializer(m_serializer);
                if (const ISerializable* serObj = dynamic_cast<const ISerializable*>(&obj)) {
                    serObj->serialize(objectSerializer);
                } else {
                    THROW_EX() << "Type " << typeName << " (type_index: " << typeid(obj).name()
                        << ") is not serializable";
                }
                if (const IRegistrable* regObj = dynamic_cast<const IRegistrable*>(&obj)) {
                    objectSerializer << REG_NAME_TAG << regObj->name();
                }
                if (const IDrawable* drawObj = dynamic_cast<const IDrawable*>(&obj)) {
                    objectSerializer << VISIBLE_TAG << drawObj->isVisible();
                }
                m_serializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't serialize object " << m_name << ". Reason: " << ex.what();
            }
            return Serializer(m_serializer);
        }
        
        template <typename T>
        typename std::enable_if<std::is_base_of<IObject, T>::value, Serializer>::type operator<<(const std::shared_ptr<T>& obj) const
        {
            if (!obj) {
                m_serializer->startObject(m_name);
                m_serializer->writeBool(EMPTY_TAG, true);
                m_serializer->finishObject();
                return Serializer(m_serializer);
            }
            return operator<<(*obj);
        }

        template <typename T>
        Serializer operator<<(const std::vector<T>& collection) const
        {
            try {
                m_serializer->startObject(m_name);
                m_serializer->writeInt(COLLECTION_SIZE_TAG, static_cast<int>(collection.size()));
                writeArray(COLLECTION_ELEMENTS_TAG, collection.begin(), collection.end(),
                    &internal::deref<std::vector<T>::const_iterator>, SerializationTag::Array);
                m_serializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't serialize collection " << m_name << ". Reason: " << ex.what();
            }
            return Serializer(m_serializer);
        }

        template <typename K, typename V>
        Serializer operator<<(const std::map<K, V>& m) const
        {
            return serializeMap<K, V>(m);
        }
        
        template <typename K, typename V>
        Serializer operator<<(const std::unordered_map<K, V>& m) const
        {
            return serializeMap<K, V>(m);
        }

    private:
        void writeMatrix(const Matrix2& m) const
        {
            m_serializer->writeFloat("", m.get(0, 0));
            m_serializer->writeFloat("", m.get(0, 1));
            m_serializer->writeFloat("", m.get(1, 0));
            m_serializer->writeFloat("", m.get(1, 1));
        }

        void writeVector(const Vec2& v) const
        {
            m_serializer->writeFloat("", v.x);
            m_serializer->writeFloat("", v.y);
        }
        
        template <typename Iter, typename DerefFunc>
        void writeArray(
            const std::string& name,
            Iter begin, Iter end, const DerefFunc& derefFunc,
            SerializationTag::Type tag) const
        {
            m_serializer->startArray(name, tag);
            ValueSerializer elemsSerializer(m_serializer, "");
            for (auto it = begin; it != end; ++it)
                elemsSerializer << derefFunc(it);
            m_serializer->finishArray();
        }
        
        template <typename K, typename V, typename Map>
        Serializer serializeMap(Map& m) const
        {
            try {
                m_serializer->startObject(m_name);
                m_serializer->writeInt(COLLECTION_SIZE_TAG, static_cast<int>(m.size()));
                writeArray(MAP_KEYS_TAG, m.begin(), m.end(),
                    &internal::derefKey<K, Map::const_iterator>, SerializationTag::Keys);
                writeArray(MAP_VALUES_TAG, m.begin(), m.end(),
                    &internal::derefValue<V, Map::const_iterator>, SerializationTag::Values);
                m_serializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't serialize map " << m_name << ". Reason: " << ex.what();
            }
            return Serializer(m_serializer);
        }

        ISerializer* m_serializer;
        std::string m_name;
    };

    ValueSerializer operator<<(const std::string& name) const
    {
        return ValueSerializer(m_serializer, name);
    }

    ISerializer* innerSerializer() const
    {
        return m_serializer;
    }

private:
    ISerializer* m_serializer;
};

} }