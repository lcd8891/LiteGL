#pragma once
#include <LiteGL/types.hpp>
#include <glm/mat4x4.hpp>

namespace LiteAPI{
    namespace Screenmgr{
        extern glm::mat4 screenView;
        void recalc_screenView();
        void initialize();
        void finalize();
    }
}