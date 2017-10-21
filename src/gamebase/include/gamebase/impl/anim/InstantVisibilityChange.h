/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/anim/IAnimation.h>

namespace gamebase { namespace impl {

class GAMEBASE_API InstantVisibilityChange : public IAnimation {
public:
	InstantVisibilityChange(const std::string& objName, bool value)
		: m_objName(objName)
		, m_value(value)
	{}

	virtual void load(const PropertiesRegister& props) override;

	virtual void start() override
	{
		m_done = false;
	}

	virtual Time step(Time t) override;

	virtual bool isFinished() const override
	{
		return m_done;
	}

protected:
	std::string m_objName;
	IDrawable* m_drawable;
	bool m_value;
	bool m_done;
};

} }
