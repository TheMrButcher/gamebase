#include "DesignViewBuilder.h"
#include "tools.h"
#include <gamebase/engine/TransparentLinearLayoutSkin.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/AnimatedTextEditSkin.h>
#include <gamebase/engine/TextEdit.h>
#include <boost/lexical_cast.hpp>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<LinearLayout> createPropertiesListLayout()
{
    auto skin = std::make_shared<TransparentLinearLayoutSkin>(
        std::make_shared<OffsettedBox>(), Direction::Vertical);
    skin->setPadding(2.0f);
    skin->setAdjustSize(false);
    return std::make_shared<LinearLayout>(
        std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center), skin);
}

std::shared_ptr<LinearLayout> createPropertyLayout()
{
    auto skin = std::make_shared<TransparentLinearLayoutSkin>(
        std::make_shared<OffsettedBox>(), Direction::Horizontal);
    skin->setPadding(5.0f);
    skin->setAdjustSize(true);
    return std::make_shared<LinearLayout>(nullptr, skin);
}

std::shared_ptr<PressableButton> createObjectButton(const std::string& name)
{
    auto skin = createButtonSkin(200.0f, 20.0f, name);
    auto button = std::make_shared<PressableButton>(nullptr, skin);
    button->setUnpressOnFocusLost(false);
    return button;
}

std::shared_ptr<StaticLabel> createLabel(const std::string& text)
{
    auto label = std::make_shared<StaticLabel>(
        std::make_shared<FixedBox>(200.0f, 20.0f));
    AlignProperties alignProperties;
    alignProperties.horAlign = HorAlign::Center;
    alignProperties.vertAlign = VertAlign::Center;
    alignProperties.enableStacking = false;
    label->setAdjustSize(false);
    label->setText(text + ":");
    return label;
}

std::shared_ptr<TextEdit> createTextEdit()
{
     auto skin = std::make_shared<AnimatedTextEditSkin>(
        std::make_shared<FixedBox>(200.0f, 20.0f));

    skin->label().setRelativeBox(std::make_shared<RelativeBox>(
        RelativeValue(RelType::ValueMinusPixels, 8.0f),
        RelativeValue(RelType::ValueMinusPixels, 8.0f),
        std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));

    auto border = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    border->setColor(Color(0, 0, 0));
    skin->addElement(border);

    auto fill = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));
    fill->setColor(Color(1.0f, 1.0f, 1.0f));
    skin->addElement(fill);

    return std::make_shared<TextEdit>(nullptr, skin);
}

class PrimitiveArrayElementSuffix {
public:
    PrimitiveArrayElementSuffix()
    {
        m_suffixes[SerializationTag::Vec2].push_back(".x");
        m_suffixes[SerializationTag::Vec2].push_back(".y");

        m_suffixes[SerializationTag::Matrix2].push_back("[0, 0]");
        m_suffixes[SerializationTag::Matrix2].push_back("[0, 1]");
        m_suffixes[SerializationTag::Matrix2].push_back("[1, 0]");
        m_suffixes[SerializationTag::Matrix2].push_back("[1, 1]");

        m_suffixes[SerializationTag::Transform2].push_back(".m[0, 0]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[0, 1]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[1, 0]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[1, 1]");
        m_suffixes[SerializationTag::Transform2].push_back(".tx");
        m_suffixes[SerializationTag::Transform2].push_back(".ty");

        m_suffixes[SerializationTag::BoundingBox].push_back(".left");
        m_suffixes[SerializationTag::BoundingBox].push_back(".bottom");
        m_suffixes[SerializationTag::BoundingBox].push_back(".right");
        m_suffixes[SerializationTag::BoundingBox].push_back(".top");

        m_suffixes[SerializationTag::Color].push_back(".r");
        m_suffixes[SerializationTag::Color].push_back(".g");
        m_suffixes[SerializationTag::Color].push_back(".b");
        m_suffixes[SerializationTag::Color].push_back(".a");
    }

    const std::string& get(SerializationTag::Type tag, size_t index) const
    {
        return m_suffixes.at(tag).at(index);
    }

private:
    std::map<SerializationTag::Type, std::vector<std::string>> m_suffixes;
};

const PrimitiveArrayElementSuffix PRIMITIVE_ARRAY_ELEMENT_SUFFIX;
}

DesignViewBuilder::DesignViewBuilder(
    TreeView& treeView,
    ObjectsSelector& propertiesMenu,
    int rootID)
    : m_treeView(treeView)
    , m_propertiesMenu(propertiesMenu)
{
    m_nodeIDs.push_back(rootID);
}

void DesignViewBuilder::writeFloat(const std::string& name, float f)
{
    std::string fullName = name;
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        fullName = m_curName + PRIMITIVE_ARRAY_ELEMENT_SUFFIX.get(
            m_arrayTypes.back(), m_primitiveElementIndex++);
    }
    addProperty(fullName, boost::lexical_cast<std::string>(f));
}

void DesignViewBuilder::writeDouble(const std::string& name, double d)
{
    addProperty(name, boost::lexical_cast<std::string>(d));
}

void DesignViewBuilder::writeInt(const std::string& name, int i)
{
    addProperty(name, boost::lexical_cast<std::string>(i));
}
void DesignViewBuilder::writeUInt(const std::string& name, unsigned int i)
{
    addProperty(name, boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeInt64(const std::string& name, int64_t i)
{
    addProperty(name, boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeUInt64(const std::string& name, uint64_t i)
{
    addProperty(name, boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeBool(const std::string& name, bool b)
{
    addProperty(name, boost::lexical_cast<std::string>(b));
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == TYPE_NAME_TAG) {
        m_objTypes.back() = ObjType::Object;
        std::string objName =
            (m_curName.empty() ? std::string("") : (m_curName + " : ")) + value;
        auto button = createObjectButton(objName);
        auto id = m_treeView.addObject(m_nodeIDs.back(), button);
        auto propertiesLayout = createPropertiesListLayout();
        m_propertiesMenu.addObject(id, propertiesLayout);
        m_properties.push_back(propertiesLayout.get());
        button->setCallback(std::bind(&ObjectsSelector::select, &m_propertiesMenu, id));
        m_nodeIDs.push_back(id);
        return;
    }

    addProperty(name, value);
}

void DesignViewBuilder::startObject(const std::string& name)
{
    m_objTypes.push_back(ObjType::Unknown);
    m_curName = name;
}

void DesignViewBuilder::finishObject()
{
    if (m_objTypes.back() == ObjType::Object) {
        m_nodeIDs.pop_back();
        m_properties.pop_back();
    }
    m_objTypes.pop_back();
}

void DesignViewBuilder::startArray(const std::string& name, SerializationTag::Type tag)
{
    m_arrayTypes.push_back(tag);
    if (tag != SerializationTag::Array && tag != SerializationTag::Keys && tag != SerializationTag::Values) {
        m_objTypes.push_back(ObjType::PrimitiveArray);
        m_primitiveElementIndex = 0;
        m_curName = name;
    }
}

void DesignViewBuilder::finishArray()
{
    if (m_objTypes.back() == ObjType::PrimitiveArray)
        m_objTypes.pop_back();
    m_arrayTypes.pop_back();
}

void DesignViewBuilder::addProperty(const std::string& name, const std::string& initialValue)
{
    auto layout = createPropertyLayout();
    //layout->setName(name);
    layout->addObject(createLabel(name));
    auto textEdit = createTextEdit();
    textEdit->setName("value");
    textEdit->setText(initialValue);
    layout->addObject(textEdit);
    m_properties.back()->addObject(layout);
}

} }
