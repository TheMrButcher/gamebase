#pragma once

#define MOVABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete; \
    ClassName(ClassName&&) = default; \
    ClassName& operator=(ClassName&&) = default;
