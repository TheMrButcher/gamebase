/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "DesignModel.h"
#include <gamebase/ui/Layout.h>

namespace gamebase { namespace editor {

class EnumPresentation;
class Presentation;
class TypePresentation;
class IPropertyPresentation;

class IProperty {
public:
    IProperty();
    virtual ~IProperty() {}

    void setModelNodeID(int id) { m_modelNodeID = id; }
    void setName(const std::string& name) { m_name = name; }
    const std::string& name() const { return m_name; }
    void setNameUI(const std::string& name) { m_nameUI = name; }
    void setHidden(bool value) { m_isHidden = value; }
    bool isHidden() const { return m_isHidden; }
    void setFictive(bool value) { m_isFictive = value; }
    void setCallback(const std::function<void()>& callback) { m_callback = callback; }
    bool isNameProperty() const;

    virtual void sync()
    {
        if (m_isHidden || !m_layout)
            return;
        syncImpl();
    }

    Layout layout() const { return m_layout; }

    virtual void attach(Layout parentLayout, const std::function<void()>& defaultCallback)
    {
        if (m_isHidden)
            return;
        const auto& callback = m_callback ? m_callback : defaultCallback;
        attachImpl(parentLayout, [this, callback]()
        {
            sync();
            callback();
        });
    }

    virtual void detach()
    {
        if (m_isHidden)
            return;
        syncImpl();
        detachImpl();
        m_layout = Layout();
    }

    virtual int addUpdater(DesignModel& model)
    {
        if (m_isFictive)
            return -1;
        return addUpdaterImpl(model);
    }

    virtual DesignModel::UpdateModelFunc makeUpdater() = 0;

    virtual std::string toString() const = 0;

protected:
    virtual void attachImpl(Layout parentLayout, const std::function<void()>& callback) = 0;
    virtual void detachImpl() = 0;
    virtual void syncImpl() = 0;
    virtual int addUpdaterImpl(DesignModel& model)
    {
        return model.addUpdater(m_modelNodeID, makeUpdater());
    }

protected:
    std::string m_name;
    std::string m_nameUI;
    int m_modelNodeID;
    bool m_isHidden;
    bool m_isFictive;
    std::function<void()> m_callback;
    Layout m_layout;
};

class IColorProperty : public IProperty {
public:
    virtual void setColorComponent(size_t index, float value) = 0;
};

class IClassNameProperty : public IProperty {
public:
    virtual boost::optional<std::string> className() const = 0;
};

typedef std::vector<std::shared_ptr<IProperty>> PropertyList;

template <typename DataType>
std::shared_ptr<IProperty> createSimpleProperty(const DataType& value);

std::shared_ptr<IColorProperty> createColorProperty();
std::shared_ptr<IProperty> createEnumProperty(
    const EnumPresentation* enumPresentation, int value);
std::shared_ptr<IProperty> createBoolProperty(bool value);
std::shared_ptr<IProperty> createFontProperty(const std::string& value);
std::shared_ptr<IProperty> createImagePathProperty(const std::string& value);

std::shared_ptr<IClassNameProperty> createEmptyClassNameProperty(
    const Presentation* presentation,
    const IPropertyPresentation* propertyPresentation);
std::shared_ptr<IClassNameProperty> createClassNameProperty(
    const Presentation* presentation,
    const IPropertyPresentation* propertyPresentation,
    const TypePresentation* typePresentation,
    const std::string& value);

std::shared_ptr<IProperty> createStaticProperty(const std::string& value);

} }
