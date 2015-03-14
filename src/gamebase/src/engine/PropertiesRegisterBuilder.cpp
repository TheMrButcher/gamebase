#include <stdafx.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/engine/IRegistrable.h>
#include <gamebase/engine/ValueLink.h>
#include <gamebase/engine/ValueNotifyingLink.h>

namespace gamebase {

PropertiesRegisterBuilder::PropertiesRegisterBuilder()
    : m_anonymousIndex(std::make_shared<unsigned int>(0))
    , m_current(nullptr)
{}

std::string PropertiesRegisterBuilder::registrableName(const std::string& name)
{
    if (!name.empty())
        return name;
    std::ostringstream oss;
    oss << "obj" << (*m_anonymousIndex)++;
    return oss.str();
}

void PropertiesRegisterBuilder::registerObject(IObject* obj)
{
    if (auto* registrable = dynamic_cast<IRegistrable*>(obj)) {
        auto regName = registerInCurrent(registrable->name(), obj);
        buildRegister(regName, registrable);
    } else {
        registerInCurrent("", obj);
    }
}

void PropertiesRegisterBuilder::registerObject(const std::string& name, IObject* obj)
{
    auto regName = registerInCurrent(name, obj);
    if (auto* registrable = dynamic_cast<IRegistrable*>(obj))
        buildRegister(regName, registrable);
}


void PropertiesRegisterBuilder::registerProperty(
    const std::string& name,
    Color* prop,
    const std::function<void()>& notifier)
{
    registerProperty<Color>(name, prop, notifier);
    registerProperty<float>(name + "R", &prop->r, notifier);
    registerProperty<float>(name + "G", &prop->g, notifier);
    registerProperty<float>(name + "B", &prop->b, notifier);
    registerProperty<float>(name + "A", &prop->a, notifier);
}

void PropertiesRegisterBuilder::registerProperty(
    const std::string& name,
    Vec2* prop,
    const std::function<void()>& notifier)
{
    registerProperty<Vec2>(name, prop, notifier);
    registerProperty<float>(name + "X", &prop->x, notifier);
    registerProperty<float>(name + "Y", &prop->y, notifier);
}

PropertiesRegisterBuilder::PropertiesRegisterBuilder(
    const std::shared_ptr<unsigned int>& anonIndex,
    IRegistrable* current)
    : m_anonymousIndex(anonIndex)
    , m_current(current)
{}

std::string PropertiesRegisterBuilder::registerInCurrent(
    const std::string& name, IObject* obj)
{
    auto regName = registrableName(name);
    if (m_current)
        m_current->properties().add(regName, obj);
    return regName;
}

void PropertiesRegisterBuilder::buildRegister(const std::string& regName, IRegistrable* registrable)
{
    PropertiesRegisterBuilder subbuilder(m_anonymousIndex, registrable);
    auto& props = registrable->properties();
    props.m_registrableName = regName;
    props.m_current = registrable;
    props.m_parent = m_current;
    registrable->registerObject(&subbuilder);
}
}
