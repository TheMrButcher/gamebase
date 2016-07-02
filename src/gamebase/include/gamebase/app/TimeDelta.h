#pragma once

#include <gamebase/impl/app/TimeState.h>

namespace gamebase {

inline float timeDelta() { return impl::TimeState::realTime().delta / 1000.0f; }

}
