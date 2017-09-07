/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

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
#include <gamebase/impl/geom/BoundingBox.h>
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

class Serializer {
public:
    Serializer(ISerializer* serializer, SerializationMode mode)
        : m_serializer(serializer)
        , m_mode(mode)
    {}

    class ValueSerializer {
    public:
        ValueSerializer(ISerializer* serializer, const std::string& name, SerializationMode mode)
            : m_serializer(serializer)
            , m_name(name)
            , m_mode(mode)
        {}

        Serializer operator<<(float f) const
        {
            m_serializer->writeFloat(m_name, f);
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(double d) const
        {
            m_serializer->writeDouble(m_name, d);
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(int i) const
        {
            m_serializer->writeInt(m_name, i);
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(unsigned int i) const
        {
            m_serializer->writeUInt(m_name, i);
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(int64_t i) const
        {
            m_serializer->writeInt64(m_name, i);
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(uint64_t i) const
        {
            m_serializer->writeUInt64(m_name, i);
            return Serializer(m_serializer, m_mode);
        }
        
        Serializer operator<<(bool b) const
        {
            m_serializer->writeBool(m_name, b);
            return Serializer(m_serializer, m_mode);
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
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const Vec2& v) const
        {
            m_serializer->startArray(m_name, SerializationTag::Vec2);
            writeVector(v);
            m_serializer->finishArray();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const Matrix2& m) const
        {
            m_serializer->startArray(m_name, SerializationTag::Matrix2);
            writeMatrix(m);
            m_serializer->finishArray();
            return Serializer(m_serializer, m_mode);
        }
        
        Serializer operator<<(const Transform2& t) const
        {
            m_serializer->startArray(m_name, SerializationTag::Transform2);
            writeMatrix(t.matrix);
            writeVector(t.offset);
            m_serializer->finishArray();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const BoundingBox& b) const
        {
            m_serializer->startArray(m_name, SerializationTag::BoundingBox);
            writeVector(b.bottomLeft);
            writeVector(b.topRight);
            m_serializer->finishArray();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const Color& c) const
        {
            m_serializer->startArray(m_name, SerializationTag::Color);
            m_serializer->writeFloat("", c.r);
            m_serializer->writeFloat("", c.g);
            m_serializer->writeFloat("", c.b);
            m_serializer->writeFloat("", c.a);
            m_serializer->finishArray();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const RelativeValue& relVal) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "RelativeValue");
            Serializer relValSerializer(m_serializer, m_mode);
            relValSerializer << "type" << relVal.type();
            relValSerializer << "value" << relVal.value();
            m_serializer->finishObject();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const TypedTime& time) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "TypedTime");
            Serializer timeSerializer(m_serializer, m_mode);
            timeSerializer << "type" << time.type << "value" << time.value;
            m_serializer->finishObject();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const FontDesc& fontDesc) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "FontDesc");
            Serializer fontDescSerializer(m_serializer, m_mode);
            fontDescSerializer << "family" << fontDesc.fontFamily << "size" << fontDesc.size;
            m_serializer->finishObject();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const AlignProperties& props) const
        {
            m_serializer->startObject(m_name);
            m_serializer->writeString(TYPE_NAME_TAG, "AlignProperties");
            Serializer propsSerializer(m_serializer, m_mode);
            propsSerializer
				<< "horAlign" << props.horAlign
				<< "vertAlign" << props.vertAlign
				<< "enableStacking" << props.enableStacking
				<< "font" << props.font;
            m_serializer->finishObject();
            return Serializer(m_serializer, m_mode);
        }

        Serializer operator<<(const IObject& obj) const
        {
            try {
                m_serializer->startObject(m_name);
                const auto& typeTraits = SerializableRegister::instance().typeTraits(typeid(obj));
                m_serializer->writeString(TYPE_NAME_TAG, typeTraits.typeName);
                Serializer objectSerializer(m_serializer, m_mode);
                if (const IRegistrable* regObj = dynamic_cast<const IRegistrable*>(&obj)) {
                    auto name = regObj->name();
                    if (m_mode == SerializationMode::ForcedFull || !name.empty())
                        objectSerializer << REG_NAME_TAG << regObj->name();
                }
                if (const IDrawable* drawObj = dynamic_cast<const IDrawable*>(&obj)) {
                    bool isVisible = drawObj->isVisible();
                    if (m_mode == SerializationMode::ForcedFull || !isVisible)
                        objectSerializer << VISIBLE_TAG << drawObj->isVisible();
                }
                if (const auto& serialize = typeTraits.serialize) {
                    serialize(&obj, objectSerializer);
                } else {
                    THROW_EX() << "Type " << typeTraits.typeName << " (type_index: " << typeid(obj).name()
                        << ") is not serializable";
                }
                m_serializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't serialize object " << m_name << ". Reason: " << ex.what();
            }
            return Serializer(m_serializer, m_mode);
        }
        
        template <typename T>
        typename std::enable_if<std::is_base_of<IObject, T>::value, Serializer>::type operator<<(const std::shared_ptr<T>& obj) const
        {
            if (!obj) {
                m_serializer->startObject(m_name);
                m_serializer->writeBool(EMPTY_TAG, true);
                m_serializer->finishObject();
                return Serializer(m_serializer, m_mode);
            }
            return operator<<(*obj);
        }

        template <typename T>
        Serializer operator<<(const std::vector<T>& collection) const
        {
            try {
                writeArray(m_name, collection.begin(), collection.end(),
                    [](auto it) { return *it; }, SerializationTag::Array);
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't serialize collection " << m_name << ". Reason: " << ex.what();
            }
            return Serializer(m_serializer, m_mode);
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
            ValueSerializer elemsSerializer(m_serializer, "", m_mode);
            for (auto it = begin; it != end; ++it)
                elemsSerializer << derefFunc(it);
            m_serializer->finishArray();
        }
        
        template <typename K, typename V, typename Map>
        Serializer serializeMap(Map& m) const
        {
            try {
                m_serializer->startArray(m_name, SerializationTag::Map);
                for (const auto& pair : m) {
                    m_serializer->startObject("");

                    ValueSerializer keySerializer(m_serializer, MAP_KEY_TAG, m_mode);
                    keySerializer << pair.first;

                    ValueSerializer valueSerializer(m_serializer, MAP_VALUE_TAG, m_mode);
                    valueSerializer << pair.second;

                    m_serializer->finishObject();
                }
                m_serializer->finishArray();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't serialize map " << m_name << ". Reason: " << ex.what();
            }
            return Serializer(m_serializer, m_mode);
        }

        ISerializer* m_serializer;
        std::string m_name;
        SerializationMode m_mode;
    };

    ValueSerializer operator<<(const std::string& name) const
    {
        return ValueSerializer(m_serializer, name, m_mode);
    }

    ISerializer* innerSerializer() const
    {
        return m_serializer;
    }

    SerializationMode mode() const
    {
        return m_mode;
    }

private:
    ISerializer* m_serializer;
    SerializationMode m_mode;
};

} }
