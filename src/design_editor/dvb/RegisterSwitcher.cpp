/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RegisterSwitcher.h"
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/relbox/SquareBox.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/relbox/ComplexBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/SerializableRegister.h>

namespace gamebase { namespace editor {
namespace {
typedef std::function<void(const impl::IObject*, impl::Serializer&)> SFunc;
typedef std::function<std::unique_ptr<impl::IObject>(impl::Deserializer&)> DFunc;

RegisterSwitcher::RegType currentRegType;

template <typename T>
struct ClassRegistrar {
    ClassRegistrar(
        const std::string& typeName,
        const DFunc& deserialize,
        const SFunc& serialize = nullptr)
        : typeName(typeName)
        , deserialize(deserialize)
        , serialize(serialize ? serialize : &impl::internal::defaultSerialize)
    {}

    void operator()() const
    {
        impl::SerializableRegister::instance().registerType<T>(typeName, deserialize, serialize);
    }

    std::string typeName;
    DFunc deserialize;
    SFunc serialize;
};

class RegisterSnapshot {
public:
    RegisterSnapshot(RegisterSwitcher::RegType targetRegType)
        : targetRegType_(targetRegType)
    {}

    template <typename T>
    void add()
    {
        add<T>(impl::SerializableRegister::instance().typeTraits<T>().deserialize);
    }

    template <typename T>
    void add(const DFunc& deserialize)
    {
        auto typeName = impl::SerializableRegister::instance().typeName<T>();
        m_registrars.push_back(ClassRegistrar<T>(typeName, deserialize));
    }

    template <typename T>
    void add(const DFunc& deserialize, const SFunc& serialize)
    {
        auto typeName = impl::SerializableRegister::instance().typeName<T>();
        add<T>(typeName, deserialize, serialize);
    }

    template <typename T>
    void add(const std::string& typeName, const DFunc& deserialize, const SFunc& serialize)
    {
        m_registrars.push_back(ClassRegistrar<T>(typeName, deserialize, serialize));
    }

    void apply() const
    {
        currentRegType = targetRegType_;
        for (auto it = m_registrars.begin(); it != m_registrars.end(); ++it) {
            (*it)();
        }
    }

protected:
    std::vector<std::function<void()>> m_registrars;
    RegisterSwitcher::RegType targetRegType_;
};

impl::RelativeValue makeValue(impl::RelType::Enum type, float value)
{
    return impl::RelativeValue(type, value);
}

impl::RelativeValue identic()
{
    return makeValue(impl::RelType::Identic, 0);
}

impl::RelativeValue pixels(float value)
{
    return makeValue(impl::RelType::Pixels, value);
}

impl::RelativeValue zeroValue()
{
    return pixels(0);
}

void serializeFixedOffsetAsAligning(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto offset = dynamic_cast<const impl::FixedOffset*>(obj)) {
        Vec2 v = offset->get();
        s   << "horAlign" << impl::HorAlign::None
            << "vertAlign" << impl::VertAlign::None
            << "horOffset" << pixels(v.x)
            << "vertOffset" << pixels(v.y);
    } else {
        THROW_EX() << "Serialzier expected FixedOffset as input";
    }
}

std::unique_ptr<impl::IObject> deserializeOffset(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    std::unique_ptr<IObject> result;
    if (horAlign == HorAlign::None && vertAlign == VertAlign::None
        && horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
        result.reset(new FixedOffset(horOffset.value(), vertOffset.value()));
    } else {
        result.reset(new AligningOffset(horAlign, vertAlign, horOffset, vertOffset));
    }
    return std::move(result);
}

void serializeOffsets(
    const impl::RelativeValue& horOffset,
    const impl::RelativeValue& vertOffset,
    impl::Serializer& s)
{
    s << "horOffset" << horOffset << "vertOffset" << vertOffset;
}

void serializeBoxOffset(
    const impl::IRelativeOffset* offset, impl::Serializer& s)
{
    auto horOffset = zeroValue();
    auto vertOffset = zeroValue();
    if (offset) {
        if (auto aligningOffset = dynamic_cast<const impl::AligningOffset*>(offset)) {
            if (aligningOffset->horAlign() != impl::HorAlign::None
                || aligningOffset->vertAlign() != impl::VertAlign::None)
                std::cerr << "Warning: offset of box shouldn't contain any alignment" << std::endl;
            horOffset = aligningOffset->horOffset();
            vertOffset = aligningOffset->vertOffset();
        } else if (auto fixedOffset = dynamic_cast<const impl::FixedOffset*>(offset)) {
            Vec2 v = fixedOffset->get();
            horOffset = pixels(v.x);
            vertOffset = pixels(v.y);
        } else {
            std::cerr << "Warning: unknown type of offset! Replaced with zero" << std::endl;
        }
    }
    serializeOffsets(horOffset, vertOffset, s);
}

void serializeRelativeBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::RelativeBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << box->widthValue()
            << "height" << box->heightValue();
        serializeBoxOffset(box->offset().get(), s);
    } else {
        THROW_EX() << "Serialzier expected RelativeBox as input";
    }
}

void serializeOffsettedBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    std::cerr << "Warning: detected deprecated OffsettedBox" << std::endl;
    if (auto box = dynamic_cast<const impl::OffsettedBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << identic()
            << "height" << identic();
        serializeBoxOffset(box->offset().get(), s);
    } else {
        THROW_EX() << "Serialzier expected OffsettedBox as input";
    }
}

void serializeSquareBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::SquareBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Min
            << "heightSource" << impl::BoxSize::Min
            << "width" << identic()
            << "height" << identic();
        serializeOffsets(zeroValue(), zeroValue(), s);
    } else {
        THROW_EX() << "Serialzier expected SquareBox as input";
    }
}

void serializeFixedBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::FixedBox*>(obj)) {
        auto bb = box->count(Box());
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << pixels(bb.width())
            << "height" << pixels(bb.height());
        Vec2 offset = bb.center();
        serializeOffsets(pixels(offset.x), pixels(offset.y), s);
    } else {
        THROW_EX() << "Serialzier expected FixedBox as input";
    }
}

void serializePixelBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::PixelBox*>(obj)) {
        auto bb = box->count(Box());
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << pixels(bb.width())
            << "height" << pixels(bb.height());
        serializeOffsets(zeroValue(), zeroValue(), s);
    } else {
        THROW_EX() << "Serialzier expected PixelBox as input";
    }
}

bool isZero(const impl::RelativeValue& value)
{
    return value.type() == impl::RelType::Pixels && value.value() == 0;
}

bool isIdentic(const impl::RelativeValue& value)
{
    return value.type() == impl::RelType::Identic;
}

std::unique_ptr<impl::IObject> deserializeBox(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(BoxSize::Type, widthSource);
    DESERIALIZE(BoxSize::Type, heightSource);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    std::unique_ptr<IObject> result;
    if (isZero(horOffset) && isZero(vertOffset)) {
        if (width.type() == height.type()) {
            if (widthSource == BoxSize::Min && heightSource == BoxSize::Min && isIdentic(width))
                result.reset(new SquareBox());
            if (width.type() == RelType::Pixels)
                result.reset(new PixelBox(width.value(), height.value()));
        }
    }
    
    if (!result && width.type() == RelType::Pixels && height.type() == RelType::Pixels
        && horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
        Box bb(width.value(), height.value(), Vec2(horOffset.value(), vertOffset.value()));
        result.reset(new FixedBox(bb));
    }

    if (!result && widthSource == BoxSize::Width && heightSource == BoxSize::Height) {
        std::shared_ptr<IRelativeOffset> offset;
        if (horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
            if (horOffset.value() != 0 || vertOffset.value() != 0)
                offset.reset(new FixedOffset(horOffset.value(), vertOffset.value()));
        } else {
            offset.reset(new AligningOffset(HorAlign::None, VertAlign::None, horOffset, vertOffset));
        }
        result.reset(new RelativeBox(width, height, offset));
    }

    if (!result)
        result.reset(new ComplexBox(width, height, widthSource, heightSource, horOffset, vertOffset));
    return std::move(result);
}

class ResultBuilderRegisterSnapshot : public RegisterSnapshot {
public:
    static const RegisterSnapshot& instance()
    {
        static ResultBuilderRegisterSnapshot SNAPSHOT;
        return SNAPSHOT;
    }

private:
    ResultBuilderRegisterSnapshot()
        : RegisterSnapshot(RegisterSwitcher::ResultReg)
    {
        add<impl::FixedOffset>();
        add<impl::AligningOffset>(&deserializeOffset);

        add<impl::RelativeBox>();
        add<impl::OffsettedBox>();
        add<impl::SquareBox>();
        add<impl::FixedBox>();
        add<impl::PixelBox>();
        add<impl::ComplexBox>(&deserializeBox);
    }
};

class DesignViewBuilderRegisterSnapshot : public RegisterSnapshot {
public:
    static const RegisterSnapshot& instance()
    {
        static DesignViewBuilderRegisterSnapshot SNAPSHOT;
        return SNAPSHOT;
    }

private:
    DesignViewBuilderRegisterSnapshot()
        : RegisterSnapshot(RegisterSwitcher::DVBReg)
    {
        add<impl::FixedOffset>("AligningOffset", &deserializeOffset, &serializeFixedOffsetAsAligning);
        add<impl::AligningOffset>(&deserializeOffset);
        
        add<impl::RelativeBox>("ComplexBox", &deserializeBox, &serializeRelativeBoxAsComplex);
        add<impl::OffsettedBox>("ComplexBox", &deserializeBox, &serializeOffsettedBoxAsComplex);
        add<impl::SquareBox>("ComplexBox", &deserializeBox, &serializeSquareBoxAsComplex);
        add<impl::FixedBox>("ComplexBox", &deserializeBox, &serializeFixedBoxAsComplex);
        add<impl::PixelBox>("ComplexBox", &deserializeBox, &serializePixelBoxAsComplex);
        add<impl::ComplexBox>(&deserializeBox);
    }
};
}

RegisterSwitcher::RegisterSwitcher()
    : startingReg_(current())
{
    if (startingReg_ != DVBReg)
        DesignViewBuilderRegisterSnapshot::instance().apply();
}

RegisterSwitcher::~RegisterSwitcher()
{
    if (startingReg_ == ResultReg)
        ResultBuilderRegisterSnapshot::instance().apply();
}

void RegisterSwitcher::init()
{
    ResultBuilderRegisterSnapshot::instance().apply();
}

RegisterSwitcher::RegType RegisterSwitcher::current()
{
    return currentRegType;
}
} }
