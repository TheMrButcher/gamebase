/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase {

#ifdef GAMEBASE_EXPORTS
#define GAMEBASE_API __declspec(dllexport) 
#else
#define GAMEBASE_API __declspec(dllimport) 
#endif

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4355)
#pragma warning(disable : 4250)

}
