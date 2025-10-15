#pragma once
#include <string>

namespace ENGINE_RES{
    std::string rect_code_vert = R"(
        #version 330 core
        layout(location=0) in vec2 v_pos;
        layout(location=1) in vec4 v_color;
        out vec4 f_color;
        void main(){
            f_color = v_color;
            gl_Position = vec4(v_pos,0,1);
        }
    )";
    std::string rect_code_frag = R"(
        #version 330 core
        in vec4 f_color;
        out vec4 o_color;
        void main(){
            o_color = f_color;
        }
    )";
}