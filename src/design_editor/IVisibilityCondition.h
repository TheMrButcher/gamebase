/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <string>
#include <memory>

namespace gamebase { namespace editor {

struct SharedContext;
struct Properties;

class IVisibilityCondition {
public:
    IVisibilityCondition(const std::string& name)
        : m_name(name)
    {}

    virtual ~IVisibilityCondition() {}

    const std::string& name() const { return m_name; }

    virtual bool allowShow(
        const SharedContext& context,
        const std::shared_ptr<Properties>& props) const = 0;
    
    static bool allowShow(
        const IVisibilityCondition* cond,
        const SharedContext& context,
        const std::shared_ptr<Properties>& props)
    {
        if (!cond)
            return true;
        return cond->allowShow(context, props);
    }

private:
    std::string m_name;
};

} }
