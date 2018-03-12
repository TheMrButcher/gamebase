/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "TypePresentation.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/JsonDeserializer.h>

namespace gamebase { namespace editor {

void TypePresentation::serialize(impl::Serializer& s) const
{
	s << "name" << name << "nameInUI" << nameInUI;
	
	if (s.mode() == impl::SerializationMode::ForcedFull || isAbstract)
		s << "isAbstract" << isAbstract;

	s << "parents" << parents << "properties" << properties;

	if (s.mode() == impl::SerializationMode::ForcedFull || !pathToPatternValue.empty())
        s << "pathToPatternValue" << pathToPatternValue;
}

std::unique_ptr<impl::IObject> deserializeTypePresentation(impl::Deserializer& deserializer)
{
    std::unique_ptr<TypePresentation> result(new TypePresentation());
	deserializer >> "name" >> result->name >> "nameInUI" >> result->nameInUI
		>> "parents" >> result->parents >> "properties" >> result->properties;
	if (deserializer.hasMember("isAbstract"))
		deserializer >> "isAbstract" >> result->isAbstract;
	if (deserializer.hasMember("pathToPatternValue"))
		deserializer >> "pathToPatternValue" >> result->pathToPatternValue;
    return std::move(result);
}

REGISTER_CLASS(TypePresentation);

} }
