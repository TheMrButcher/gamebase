/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */
#include <stdafx.h>
#include <gamebase/impl/anim/ChangeFuncPtr.h>
#include "easing.h"

namespace gamebase { namespace impl {

ChangeFuncPtr getChangeFuncPtr(ChangeFunc::Type type)
{
    switch (type) {
    case ChangeFunc::Linear:           return &LinearInterpolation;
    case ChangeFunc::EaseInQuad:       return &QuadraticEaseIn;
    case ChangeFunc::EaseOutQuad:      return &QuadraticEaseOut;
    case ChangeFunc::EaseInOutQuad:    return &QuadraticEaseInOut;
    case ChangeFunc::EaseInCubic:      return &CubicEaseIn;
    case ChangeFunc::EaseOutCubic:     return &CubicEaseOut;
    case ChangeFunc::EaseInOutCubic:   return &CubicEaseInOut;
    case ChangeFunc::EaseInQuart:      return &QuarticEaseIn;
    case ChangeFunc::EaseOutQuart:     return &QuarticEaseOut;
    case ChangeFunc::EaseInOutQuart:   return &QuarticEaseInOut;
    case ChangeFunc::EaseInQuint:      return &QuinticEaseIn;
    case ChangeFunc::EaseOutQuint:     return &QuinticEaseOut;
    case ChangeFunc::EaseInOutQuint:   return &QuinticEaseInOut;
    case ChangeFunc::EaseInSine:       return &SineEaseIn;
    case ChangeFunc::EaseOutSine:      return &SineEaseOut;
    case ChangeFunc::EaseInOutSine:    return &SineEaseInOut;
    case ChangeFunc::EaseInExpo:       return &ExponentialEaseIn;
    case ChangeFunc::EaseOutExpo:      return &ExponentialEaseOut;
    case ChangeFunc::EaseInOutExpo:    return &ExponentialEaseInOut;
    case ChangeFunc::EaseInCirc:       return &CircularEaseIn;
    case ChangeFunc::EaseOutCirc:      return &CircularEaseOut;
    case ChangeFunc::EaseInOutCirc:    return &CircularEaseInOut;
    case ChangeFunc::EaseInElastic:    return &ElasticEaseIn;
    case ChangeFunc::EaseOutElastic:   return &ElasticEaseOut;
    case ChangeFunc::EaseInOutElastic: return &ElasticEaseInOut;
    case ChangeFunc::EaseInBack:       return &BackEaseIn;
    case ChangeFunc::EaseOutBack:      return &BackEaseOut;
    case ChangeFunc::EaseInOutBack:    return &BackEaseInOut;
    case ChangeFunc::EaseInBounce:     return &BounceEaseIn;
    case ChangeFunc::EaseOutBounce:    return &BounceEaseOut;
    case ChangeFunc::EaseInOutBounce:  return &BounceEaseInOut;
    default:                           return &LinearInterpolation;
    }
}

} }
