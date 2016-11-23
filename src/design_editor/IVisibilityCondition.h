/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <string>

namespace gamebase { namespace editor {

struct SharedContext;

class IVisibilityCondition {
public:
    IVisibilityCondition(const std::string& name)
        : m_name(name)
    {}

    virtual ~IVisibilityCondition() {}

    const std::string& name() const { return m_name; }

    virtual bool allowShow(const SharedContext& context, int nodeID) const = 0;
    
    static bool allowShow(const IVisibilityCondition* cond, const SharedContext& context, int nodeID)
    {
        if (!cond)
            return true;
        return cond->allowShow(context, nodeID);
    }

private:
    std::string m_name;
};

} }
