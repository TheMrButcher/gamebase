#pragma once

#include <gamebase/serial/SerializableRegister.h>
#include <gamebase/serial/constants.h>
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

    virtual bool readBool(const std::string& name) = 0;

    virtual std::string readString(const std::string& name) = 0;

    virtual IObject* readObject(const std::string& name) = 0;
    
    virtual void startObject(const std::string& name) = 0;

    virtual void finishObject() = 0;

    virtual void startArray(const std::string& name) = 0;

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
        
        Deserializer operator>>(bool& b) const
        {
            b = m_deserializer->readBool(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(std::string& s) const
        {
            s = m_deserializer->readString(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Vec2& v) const
        {
            m_deserializer->startArray(m_name);
            readVector(v);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Matrix2& m) const
        {
            m_deserializer->startArray(m_name);
            readMatrix(m);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(Transform2& t) const
        {
            m_deserializer->startArray(m_name);
            readMatrix(t.matrix);
            readVector(t.offset);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(BoundingBox& b) const
        {
            m_deserializer->startArray(m_name);
            ValueDeserializer vecDeserializer(m_deserializer, "");
            vecDeserializer >> b.bottomLeft;
            vecDeserializer >> b.topRight;
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Color& c) const
        {
            m_deserializer->startArray(m_name);
            c.r = m_deserializer->readFloat("");
            c.g = m_deserializer->readFloat("");
            c.b = m_deserializer->readFloat("");
            c.a = m_deserializer->readFloat("");
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }
        
        template <typename T>
        Deserializer operator>>(std::shared_ptr<T>& obj) const
        {
            m_deserializer->startObject(m_name);
            try {
                std::string typeName = m_deserializer->readString(TYPE_NAME_TAG);
                auto traits = SerializableRegister::instance().typeTraits(typeName);
                IObject* rawObj = traits.deserialize(m_deserializer);
                if (T* cnvObj = dynamic_cast<T*>(rawObj)) {
                    obj.reset(cnvObj);
                } else {
                    THROW_EX() << "Type " << typeName << " (type_index: " << traits.index.name()
                        << ") is not convertible to type " << typeid(T).name();
                }

            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize object " << m_name << ". Reason: " << ex.what();
            }
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        template <typename T>
        Deserializer operator>>(std::vector<T>& collection) const
        {
            m_deserializer->startArray(m_name);
            try {
                size_t size = static_cast<size_t>(m_deserializer->readInt(COLLECTION_SIZE_TAG));
                collection.clear();
                collection.reserve(size);
                ValueDeserializer elemsDeserializer(m_deserializer, "");
                for (size_t i = 0; i < size; ++i) {
                    T elem;
                    elemsDeserializer >> elem;
                    collection.push_back(elem);
                }
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize collection " << m_name << ". Reason: " << ex.what();
            }
            m_deserializer->finishArray();
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
        
        template <typename K, typename V, typename Map>
        Deserializer deserializeMap(Map& m) const
        {
            m_deserializer->startObject(m_name);
            try {
                ValueDeserializer keysDeserializer(m_deserializer, "keys");
                std::vector<K> keys;
                keysDeserializer >> keys;
                ValueDeserializer valuesDeserializer(m_deserializer, "values");
                std::vector<V> values;
                valuesDeserializer >> values;
                if (keys.size() != values.size())
                    THROW_EX() << "Number of keys=" << keys.size()
                        << " is not equal to number of values=" << values.size();
                m.clear();
                for (size_t i = 0; i < keys.size(); ++i)
                    m[keys[i]] = values[i];
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize map " << m_name << ". Reason: " << ex.what();
            }
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
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

}
