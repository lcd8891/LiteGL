#pragma once

#define LITEGL_VERSION_MAJOR 1
#define LITEGL_VERSION_MINOR 0
#define _LITE_GAME_ extern "C"

#ifndef __cplusplus
    #error "You using a C instead C++. Read a REQUIREMENTS in README.md"
#endif
#if __cplusplus < 201703L
    #warning "LiteGL engine fully compatible with C++ 17 and maybe some problems with older version of C++."
#endif

#include <string>

#include "system/arguments.hpp"
#include "system/logger.hpp"
#include "system/time.hpp"
#include "system/types.hpp"
#include "system/default.hpp"

#include "window/window.hpp"
#include "screen/screenmgr.hpp"
#include "glctrl/glctrl.hpp"
#include "buffer/buffers.hpp"
#include "graphics/graphics.hpp"