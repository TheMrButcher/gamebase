/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "IVisibilityCondition.h"
#include <unordered_map>
#include <memory>
#include <iostream>

namespace gamebase { namespace editor {

struct SharedContext;

class VisibilityConditions {
public:
    static VisibilityConditions& instance()
    {
        static VisibilityConditions conds;
        return conds;
    }

    void add(const std::shared_ptr<IVisibilityCondition>& cond)
    {
        m_conds[cond->name()] = cond;
    }

    bool allowShow(const std::string& condName, const SharedContext& context, int nodeID) const
    {
        return IVisibilityCondition::allowShow(get(condName), context, nodeID);
    }

    const IVisibilityCondition* get(const std::string& condName) const
    {
        if (condName.empty())
            return nullptr;
        auto it = m_conds.find(condName);
        if (it == m_conds.end()) {
            std::cerr << "Warning: unknown condition name \"" << condName << "\"" << std::endl;
            return nullptr;
        }
        return it->second.get();
    }

private:
    VisibilityConditions() {}

    std::unordered_map<std::string, std::shared_ptr<IVisibilityCondition>> m_conds;
};

} }
