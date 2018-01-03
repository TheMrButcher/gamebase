/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <dvb/IProperty.h>
#include <dvb/Styles.h>
#include <dvb/Helpers.h>
#include <dvb/ColorDialog.h>
#include <dvb/Operations.h>
#include "EnumPresentation.h"
#include "tools.h"

namespace gamebase { namespace editor {

namespace {
template <typename T>
std::string makeValueUI(const T& t)
{
    return boost::lexical_cast<std::string>(t);
}

std::string makeValueUI(double d)
{
    return toUIString(d);
}

std::string makeValueUI(const std::string& str)
{
    return str;
}

template <typename DataType>
class SimpleProperty : public IProperty {
public:
    SimpleProperty(const DataType& value)
        : m_value(value)
    {}

    virtual std::string toString() const override
    {
        return makeValueUI(m_value);
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data) { setData(data, m_name, m_value); };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& callback) override
    {
        m_layout = createPropertyLayout();

        m_textBox = createTextBox();
        m_textBox.setText(toString());
        m_textBox.setCallback(callback);

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(m_textBox);
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override
    {
        m_textBox = TextBox();
    }

    virtual void syncImpl() override
    {
        if (m_textBox)
            parseData(m_textBox.text(), m_value);
    }

private:
    DataType m_value;
    TextBox m_textBox;
};

class ColorProperty : public IColorProperty {
public:
    virtual void setColorComponent(size_t index, float value) override
    {
        switch (index) {
        case 0: m_color.r = value; break;
        case 1: m_color.g = value; break;
        case 2: m_color.b = value; break;
        case 3: m_color.a = value; break;
        }
    }

    virtual std::string toString() const override
    {
        return "color";
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data)
        {
            setData(data, "", m_color.r);
            setData(data, "", m_color.g);
            setData(data, "", m_color.b);
            setData(data, "", m_color.a);
        };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>&) override
    {
        m_layout = createPropertyLayout();

        auto colorRectButton = createColorRect();

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(colorRectButton);
        m_layout.add(createSpacer());
        parentLayout.add(m_layout);

        m_colorRect = colorRectButton.child<FilledRect>("colorRect");
        m_colorRect.setColor(m_color.intColor());

        colorRectButton.setCallback([this]()
        {
            std::function<void(const Color&)> callback =
                [this](const Color& color) mutable
            {
                m_colorRect.setColor(color);
            };
            getColorDialog().showWithColor(m_colorRect.color(), callback);
        });
    }

    virtual void detachImpl() override
    {
        m_colorRect = FilledRect();
    }

    virtual void syncImpl() override
    {
        if (m_colorRect)
            m_color = impl::makeGLColor(m_colorRect.color());
    }

private:
    FilledRect m_colorRect;
	impl::GLColor m_color;
};

class EnumProperty : public IProperty {
public:
    EnumProperty(const EnumPresentation* enumPresentation, int value)
        : m_enumPresentation(enumPresentation)
        , m_value(value)
    {}

    virtual std::string toString() const override
    {
        return m_enumPresentation->values.at(m_value);
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data) { setData(data, m_name, m_value); };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& callback) override
    {
        std::vector<int> enumValues;
        std::vector<std::string> enumValuesNames;
        for (const auto& valueAndName : m_enumPresentation->values) {
            enumValues.push_back(valueAndName.first);
            enumValuesNames.push_back(valueAndName.second);
        }

        m_layout = createPropertyLayout();

        m_comboBox = createComboBox(enumValuesNames, enumValues);
        m_comboBox.setText(m_enumPresentation->values.at(m_value));
        m_comboBox.setCallback(callback);

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(m_comboBox);
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override
    {
        m_comboBox = ComboBox();
    }

    virtual void syncImpl() override
    {
        if (m_comboBox)
            m_value = m_comboBox.selected();
    }

private:
    const EnumPresentation* m_enumPresentation;
    int m_value;
    ComboBox m_comboBox;
};

class BoolProperty : public IProperty {
public:
    BoolProperty(bool value)
        : m_value(value)
    {}

    virtual std::string toString() const override
    {
        return "boolean";
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data) { setData(data, m_name, m_value); };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& callback) override
    {
        m_layout = createPropertyLayout();

        m_checkBox = createCheckBox();
        m_checkBox.setChecked(m_value);
        m_checkBox.setCallback(callback);

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(m_checkBox);
        m_layout.add(createSpacer());
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override
    {
        m_checkBox = CheckBox();
    }

    virtual void syncImpl() override
    {
        if (m_checkBox)
            m_value = m_checkBox.isChecked();
    }

private:
    bool m_value;
    CheckBox m_checkBox;
};

class FontProperty : public IProperty {
public:
    FontProperty(const std::string& value)
        : m_value(value)
    {}

    virtual std::string toString() const override
    {
        return m_value;
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data) { setData(data, m_name, m_value); };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& callback) override
    {
        m_layout = createPropertyLayout();

        m_comboBox = createComboBox(impl::fontStorage().fontNames());
        m_comboBox.setText(m_value);
        m_comboBox.setCallback(callback);

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(m_comboBox);
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override
    {
        m_comboBox = ComboBox();
    }

    virtual void syncImpl() override
    {
        if (m_comboBox)
            m_value = m_comboBox.text();
    }

private:
    std::string m_value;
    ComboBox m_comboBox;
};

class ResourcePathProperty : public IProperty {
public:
    ResourcePathProperty(
        const std::string& value,
        const std::function<ExtFilePathDialog&()>& getDialog)
        : m_value(value)
        , m_getDialog(getDialog)
    {}

    virtual std::string toString() const override
    {
        return m_value;
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data) { setData(data, m_name, m_value); };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& onChange) override
    {
        m_layout = createPropertyLayout();

        m_textBox = createResourcePathTextBox();
        m_textBox.setText(m_value);

        auto choosePathButton = createChoosePathButton();
        choosePathButton.setCallback([this]() { chooseFile(m_textBox, m_getDialog); });

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(m_textBox);
        m_layout.add(choosePathButton);
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override
    {
        m_textBox = TextBox();
    }

    virtual void syncImpl() override
    {
        if (m_textBox)
            m_value = m_textBox.text();
    }

private:
    std::string m_value;
    std::function<ExtFilePathDialog&()> m_getDialog;
    TextBox m_textBox;
};

class ClassNameProperty : public IClassNameProperty {
public:
    ClassNameProperty(
        const Presentation* presentation,
        const IPropertyPresentation* propertyPresentation,
        const TypePresentation* typePresentation,
        const std::string& value)
        : m_presentation(presentation)
        , m_propertyPresentation(propertyPresentation)
        , m_typePresentation(typePresentation)
        , m_empty(false)
        , m_value(value)
    {}

    ClassNameProperty(
        const Presentation* presentation,
        const IPropertyPresentation* propertyPresentation)
        : m_presentation(presentation)
        , m_propertyPresentation(propertyPresentation)
        , m_typePresentation(nullptr)
        , m_empty(true)
    {}

    virtual boost::optional<std::string> className() const override
    {
        return boost::make_optional(!m_empty, m_value);
    }

    virtual std::string toString() const override
    {
        if (m_empty)
            return noneLabel();
        return m_typePresentation ? m_typePresentation->nameInUI : m_value;
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [this](auto* data)
        {
            if (m_empty) {
                (*data)[impl::EMPTY_TAG] = Json::Value(true);
                if (data->isMember(impl::TYPE_NAME_TAG))
                    data->removeMember(impl::TYPE_NAME_TAG);
            }
            else {
                (*data)[impl::TYPE_NAME_TAG] = Json::Value(m_value);
                if (data->isMember(impl::EMPTY_TAG))
                    data->removeMember(impl::EMPTY_TAG);
            }
        };
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& onChange) override
    {
        m_layout = createPropertyLayout();

        m_typesList = createTypesList(m_isFictive
            ? "No presentation"
            : (m_empty ? noneLabel() : m_value));

        if (m_isFictive) {
            if (m_empty || m_value.empty()) {
                if (m_typesList.empty()) {
                    m_empty = true;
                } else {
                    m_empty = false;
                    m_value = m_typesList.front()->name;
                    m_typePresentation = m_typesList.front();
                }
            }
        }

        if (m_empty) {
            m_comboBox.setText(noneLabel());
        } else {
            auto thisTypeName = m_value;
            auto textVariants = m_comboBox.variants();
            if (m_typePresentation) {
                auto it = std::find(textVariants.begin(), textVariants.end(),
                    m_typePresentation->nameInUI);
                if (it != textVariants.end())
                    thisTypeName = m_typePresentation->nameInUI;
            }
            m_comboBox.setText(thisTypeName);
        }
        m_comboBox.setCallback(onChange);

        m_layout.add(createLabel(m_nameUI));
        m_layout.add(m_comboBox);
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override
    {
        m_comboBox = ComboBox();
        m_typesList.clear();
    }

    virtual void syncImpl() override
    {
        if (m_comboBox) {
            if (m_typePresentation) {
                if (m_typePresentation->nameInUI == m_comboBox.text())
                    return;
            }
            m_empty = m_comboBox.text() == noneLabel();
            int selected = m_comboBox.selected();
            if (m_empty || selected < 0 || selected >= static_cast<int>(m_typesList.size())) {
                m_typePresentation = nullptr;
            } else {
                m_typePresentation = m_typesList[selected];
                if (m_typePresentation)
                    m_value = m_typePresentation->name;
            }
        }
    }

private:
    std::vector<const TypePresentation*> createTypesList(
        const std::string& variantIfNoPresentation)
    {
        std::vector<const TypePresentation*> types;
        std::vector<std::string> typesNames;
        auto objectPresentation = dynamic_cast<const ObjectPresentation*>(m_propertyPresentation);
        if (objectPresentation) {
            types = m_presentation->derivedTypesByBaseTypeName(objectPresentation->baseType);
            for (auto type : types)
                typesNames.push_back(type->nameInUI);
            if (objectPresentation->canBeEmpty)
                typesNames.push_back(noneLabel());
        } else if (variantIfNoPresentation != noneLabel()) {
            if (auto typePresentation = m_presentation->typeByName(variantIfNoPresentation)) {
                types.push_back(typePresentation);
                typesNames.push_back(typePresentation->nameInUI);
            }
        }

        if (typesNames.empty())
            typesNames.push_back(variantIfNoPresentation);

        m_comboBox = createComboBox(typesNames);
        m_comboBox.setText(typesNames[0]);
        return types;
    }

    const Presentation* m_presentation;
    const IPropertyPresentation* m_propertyPresentation;
    const TypePresentation* m_typePresentation;
    bool m_empty;
    std::string m_value;
    ComboBox m_comboBox;
    std::vector<const TypePresentation*> m_typesList;
};

class StaticProperty : public IProperty {
public:
    StaticProperty(const std::string& value)
        : m_value(value)
    {}

    virtual std::string toString() const override
    {
        return m_value;
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() override
    {
        return [](auto*) {};
    }

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>&) override
    {
        m_layout = createPropertyLayout();
        m_layout.add(createLabel(m_nameUI));
        m_layout.add(createRightLabel(m_value));
        parentLayout.add(m_layout);
    }

    virtual void detachImpl() override {}
    virtual void syncImpl() override {}
    virtual int addUpdaterImpl(DesignModel& model) override { return -1; }

private:
    std::string m_value;
};
}

IProperty::IProperty()
    : m_modelNodeID(-1)
    , m_isHidden(false)
    , m_isFictive(false)
{}

bool IProperty::isNameProperty() const
{
    return m_name == impl::REG_NAME_TAG;
}

template <typename DataType>
std::shared_ptr<IProperty> createSimpleProperty(const DataType& value)
{
    return std::make_shared<SimpleProperty<DataType>>(value);
}

template std::shared_ptr<IProperty> createSimpleProperty<int>(const int&);
template std::shared_ptr<IProperty> createSimpleProperty<unsigned int>(const unsigned int&);
template std::shared_ptr<IProperty> createSimpleProperty<int64_t>(const int64_t&);
template std::shared_ptr<IProperty> createSimpleProperty<uint64_t>(const uint64_t&);
template std::shared_ptr<IProperty> createSimpleProperty<double>(const double&);
template std::shared_ptr<IProperty> createSimpleProperty<std::string>(const std::string&);

std::shared_ptr<IColorProperty> createColorProperty()
{
    return std::make_shared<ColorProperty>();
}

std::shared_ptr<IProperty> createEnumProperty(
    const EnumPresentation* enumPresentation, int value)
{
    return std::make_shared<EnumProperty>(enumPresentation, value);
}

std::shared_ptr<IProperty> createBoolProperty(bool value)
{
    return std::make_shared<BoolProperty>(value);
}

std::shared_ptr<IProperty> createFontProperty(const std::string& value)
{
    return std::make_shared<FontProperty>(value);
}

std::shared_ptr<IProperty> createImagePathProperty(const std::string& value)
{
    return std::make_shared<ResourcePathProperty>(value, &getImagePathDialog);
}

std::shared_ptr<IProperty> createSoundPathProperty(const std::string& value)
{
    return std::make_shared<ResourcePathProperty>(value, &getSoundPathDialog);
}

std::shared_ptr<IClassNameProperty> createEmptyClassNameProperty(
    const Presentation* presentation,
    const IPropertyPresentation* propertyPresentation)
{
    return std::make_shared<ClassNameProperty>(presentation, propertyPresentation);
}

std::shared_ptr<IClassNameProperty> createClassNameProperty(
    const Presentation* presentation,
    const IPropertyPresentation* propertyPresentation,
    const TypePresentation* typePresentation,
    const std::string& value)
{
    return std::make_shared<ClassNameProperty>(
        presentation, propertyPresentation, typePresentation, value);
}

std::shared_ptr<IProperty> createStaticProperty(const std::string& value)
{
    return std::make_shared<StaticProperty>(value);
}

} }
