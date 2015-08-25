#pragma once

#include <gamebase/serial/SerializableRegister.h>
#include <gamebase/serial/constants.h>
#include <gamebase/engine/RelativeValue.h>
#include <gamebase/engine/TimeState.h>
#include <gamebase/engine/IRegistrable.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/text/AlignProperties.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/graphics/Color.h>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace gamebase {

class IDeserializer {
public:
    virtual ~IDeserializer() {}

    virtual float readFloat(const std::string& name) = 0;

    virtual double readDouble(const std::string& name) = 0;

    virtual int readInt(const std::string& name) = 0;

    virtual unsigned int readUInt(const std::string& name) = 0;

    virtual int64_t readInt64(const std::string& name) = 0;

    virtual uint64_t readUInt64(const std::string& name) = 0;

    virtual bool readBool(const std::string& name) = 0;

    virtual std::string readString(const std::string& name) = 0;
    
    virtual void startObject(const std::string& name) = 0;

    virtual void finishObject() = 0;

    virtual void startArray(const std::string& name, SerializationTag::Type tag) = 0;

    virtual void finishArray() = 0;
};

class Deserializer {
public:
    Deserializer(IDeserializer* deserializer)
        : m_deserializer(deserializer)
    {}

    class ValueDeserializer {
    public:
        ValueDeserializer(IDeserializer* deserializer, const std::string& name)
            : m_deserializer(deserializer)
            , m_name(name)
        {}

        Deserializer operator>>(float& f) const
        {
            f = m_deserializer->readFloat(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(double& d) const
        {
            d = m_deserializer->readDouble(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(int& i) const
        {
            i = m_deserializer->readInt(m_name);
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(unsigned int& i) const
        {
            i = m_deserializer->readUInt(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(int64_t& i) const
        {
            i = m_deserializer->readInt64(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(uint64_t& i) const
        {
            i = m_deserializer->readUInt64(m_name);
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(bool& b) const
        {
            b = m_deserializer->readBool(m_name);
            return Deserializer(m_deserializer);
        }

        template <typename EnumType>
        typename std::enable_if<std::is_enum<EnumType>::value, Deserializer>::type operator>>(
            EnumType& enumValue) const
        {
            enumValue = static_cast<EnumType>(m_deserializer->readInt(m_name));
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(std::string& s) const
        {
            s = m_deserializer->readString(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Vec2& v) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Vec2);
            readVector(v);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Matrix2& m) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Matrix2);
            readMatrix(m);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(Transform2& t) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Transform2);
            readMatrix(t.matrix);
            readVector(t.offset);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(BoundingBox& b) const
        {
            m_deserializer->startArray(m_name, SerializationTag::BoundingBox);
            readVector(b.bottomLeft);
            readVector(b.topRight);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Color& c) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Color);
            c.r = m_deserializer->readFloat("");
            c.g = m_deserializer->readFloat("");
            c.b = m_deserializer->readFloat("");
            c.a = m_deserializer->readFloat("");
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(RelativeValue& relVal) const
        {
            m_deserializer->startObject(m_name);
            Deserializer relValDeserializer(m_deserializer);
            RelType::Enum type;
            relValDeserializer >> "type" >> type;
            if (type == RelType::Identic) {
                relVal = RelativeValue();
            } else {
                float value;
                relValDeserializer >> "value" >> value;
                relVal = RelativeValue(type, value);
            }
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(TypedTime& time) const
        {
            m_deserializer->startObject(m_name);
            Deserializer timeDeserializer(m_deserializer);
            timeDeserializer >> "type" >> time.type >> "value" >> time.value;
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(FontDesc& fontDesc) const
        {
            m_deserializer->startObject(m_name);
            Deserializer fontDescDeserializer(m_deserializer);
            fontDescDeserializer >> "family" >> fontDesc.fontFamily >> "size" >> fontDesc.size;
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(AlignProperties& props) const
        {
            m_deserializer->startObject(m_name);
            Deserializer propsDeserializer(m_deserializer);
            propsDeserializer >> "font" >> props.font >> "horAlign" >> props.horAlign
                >> "vertAlign" >> props.vertAlign >> "enableStacking" >> props.enableStacking;
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        template <typename T>
        typename std::enable_if<std::is_base_of<IObject, T>::value, Deserializer>::type operator>>(T& obj) const
        {
            return deserializeObject<T>(obj);
        }
        
        template <typename T>
        typename std::enable_if<std::is_base_of<IObject, T>::value, Deserializer>::type operator>>(std::shared_ptr<T>& obj) const
        {
            return deserializeObject<T>(obj);
        }

        template <typename T>
        Deserializer operator>>(std::vector<T>& collection) const
        {
            try {
                m_deserializer->startObject(m_name);
                size_t size = static_cast<size_t>(m_deserializer->readInt(COLLECTION_SIZE_TAG));
                readArray(COLLECTION_ELEMENTS_TAG, collection, size, SerializationTag::Array);
                m_deserializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize collection " << m_name << ". Reason: " << ex.what();
            }
            return Deserializer(m_deserializer);
        }

        template <typename K, typename V>
        Deserializer operator>>(std::map<K, V>& m) const
        {
            return deserializeMap<K, V>(m);
        }
        
        template <typename K, typename V>
        Deserializer operator>>(std::unordered_map<K, V>& m) const
        {
            return deserializeMap<K, V>(m);
        }

    private:
        void readMatrix(Matrix2& m) const
        {
            float m00 = m_deserializer->readFloat("");
            float m01 = m_deserializer->readFloat("");
            float m10 = m_deserializer->readFloat("");
            float m11 = m_deserializer->readFloat("");
            m = Matrix2(m00, m01, m10, m11);
        }

        void readVector(Vec2& v) const
        {
            v.x = m_deserializer->readFloat("");
            v.y = m_deserializer->readFloat("");
        }

        template <typename T>
        void readArray(
            const std::string& name, std::vector<T>& collection,
            size_t size, SerializationTag::Type tag) const
        {
            collection.clear();
            collection.reserve(size);
            m_deserializer->startArray(name, tag);
            ValueDeserializer elemsDeserializer(m_deserializer, "");
            for (size_t i = 0; i < size; ++i) {
                T elem;
                elemsDeserializer >> elem;
                collection.push_back(elem);
            }
            m_deserializer->finishArray();
        }
        
        template <typename K, typename V, typename Map>
        Deserializer deserializeMap(Map& m) const
        {
            try {
                m_deserializer->startObject(m_name);
                size_t size = static_cast<size_t>(m_deserializer->readInt(COLLECTION_SIZE_TAG));
                std::vector<K> keys;
                readArray(MAP_KEYS_TAG, keys, size, SerializationTag::Keys);
                std::vector<V> values;
                readArray(MAP_VALUES_TAG, values, size, SerializationTag::Values);
                m.clear();
                for (size_t i = 0; i < keys.size(); ++i)
                    m[std::move(keys[i])] = std::move(values[i]);
                m_deserializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize map " << m_name << ". Reason: " << ex.what();
            }
            return Deserializer(m_deserializer);
        }

        template <typename T, typename U>
        Deserializer deserializeObject(U& obj) const
        {
            std::string typeName("unknown");
            try {
                m_deserializer->startObject(m_name);
                bool isEmpty = m_deserializer->readBool(EMPTY_TAG);
                if (isEmpty) {
                    resetObject(obj);
                } else {
                    typeName = m_deserializer->readString(TYPE_NAME_TAG);
                    auto traits = SerializableRegister::instance().typeTraits(typeName);
                    Deserializer objectDeserializer(m_deserializer);
                    std::unique_ptr<IObject> rawObj = traits.deserialize(objectDeserializer);
                    if (IRegistrable* regObj = dynamic_cast<IRegistrable*>(rawObj.get())) {
                        std::string objName;
                        objectDeserializer >> REG_NAME_TAG >> objName;
                        regObj->setName(objName);
                    }
                    if (IDrawable* drawObj = dynamic_cast<IDrawable*>(rawObj.get())) {
                        bool visible;
                        objectDeserializer >> VISIBLE_TAG >> visible;
                        drawObj->setVisible(visible);
                    }
                    if (T* cnvObj = dynamic_cast<T*>(rawObj.get())) {
                        rawObj.release();
                        std::unique_ptr<T> cnvObjUPtr(cnvObj);
                        setObject(cnvObjUPtr, obj);
                    } else {
                        THROW_EX() << "Type " << typeName << " (type_index: " << traits.index.name()
                            << ") is not convertible to type " << typeid(T).name();
                    }
                }
                m_deserializer->finishObject();
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize object " << m_name << " (type: " << typeName << "). Reason: " << ex.what();
            }
            return Deserializer(m_deserializer);
        }

        template <typename T>
        void setObject(std::unique_ptr<T>& cnvObj, T& obj) const
        {
            obj = std::move(*cnvObj);
        }
        
        template <typename T>
        void setObject(std::unique_ptr<T>& cnvObj, std::shared_ptr<T>& obj) const
        {
            obj.reset(cnvObj.release());
        }

        template <typename T>
        void resetObject(T& obj) const
        {
            THROW_EX() << "Can't reset reference on object (marked as empty)";
        }
        
        template <typename T>
        void resetObject(std::shared_ptr<T>& obj) const
        {
            obj.reset();
        }

        IDeserializer* m_deserializer;
        std::string m_name;
    };

    ValueDeserializer operator>>(const std::string& name) const
    {
        return ValueDeserializer(m_deserializer, name);
    }

private:
    IDeserializer* m_deserializer;
};

#define DESERIALIZE(Type, value) \
    Type value; \
    deserializer >> #value >> value

}
