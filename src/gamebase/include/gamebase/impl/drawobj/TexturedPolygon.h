/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/drawobj/TexturedPolygonVertex.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/graphics/GLTexture.h>
#include <gamebase/impl/graphics/GLBuffers.h>
#include <gamebase/impl/graphics/GLColor.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relbox/IResizable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API TexturedPolygon : public Drawable, public OffsettedPosition,
	public Registrable, public ISerializable, public IResizable {
public:
	TexturedPolygon(
		const std::shared_ptr<IRelativeBox>& box,
		const std::shared_ptr<IRelativeOffset>& position = nullptr)
		: Drawable(this)
		, OffsettedPosition(position)
		, m_box(box)
		, m_isTextureDirty(true)
		, m_isMeshDirty(false)
		, m_areBuffersDirty(false)
	{}

	const std::string& imageName() const { return m_imageName; }
	void setImageName(const std::string& name);

	void setOuterRing(const std::vector<std::shared_ptr<TexturedPolygonVertex>>& vertices);
	void setInnerRing(size_t index, const std::vector<std::shared_ptr<TexturedPolygonVertex>>& vertices);

	virtual void setFixedBox(float width, float height) override;

	virtual void loadResources() override
	{
		reload();
	}
	virtual void setBox(const BoundingBox& allowedBox) override;
	virtual BoundingBox box() const override { return m_box->get(); }
	virtual void drawAt(const Transform2& position) const override;

	virtual void registerObject(PropertiesRegisterBuilder*) override {}
	virtual void serialize(Serializer& s) const override;

private:
	void reload();

	std::shared_ptr<IRelativeBox> m_box;
	std::string m_imageName;
	BoundingBox m_parentBox;
	GLBuffers m_buffers;
	GLTexture m_texture;
	std::vector<std::shared_ptr<TexturedPolygonVertex>> m_outerRing;
	std::vector<std::vector<std::shared_ptr<TexturedPolygonVertex>>> m_innerRings;
	bool m_isTextureDirty;
	bool m_isMeshDirty;
	bool m_areBuffersDirty;
	std::vector<std::vector<Vec2>> m_vertices;
	std::vector<uint16_t> m_indices;
};

} }
