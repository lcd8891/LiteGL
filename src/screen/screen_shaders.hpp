#pragma once
#include <string>

namespace ENGINE_RES{
    std::string rect_code_vert = R"(
        #version 330 core
        layout(location=0) in vec2 v_pos;
        layout(location=1) in vec4 v_color;
        out vec4 f_color;
        uniform mat4 view;
        void main(){
            f_color = v_color;
            gl_Position = view * vec4(v_pos,1,1);
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
    std::string tex_code_vert = R"(
        #version 330 core
        layout(location=0) in vec2 v_pos;
        layout(location=1) in vec4 v_color;
        layout(location=2) in vec2 v_texCoord;
        out vec4 f_color;
        out vec2 f_texCoord;
        uniform mat4 view;
        void main(){
            f_color = v_color;
            f_texCoord = v_texCoord;
            gl_Position = view * vec4(v_pos,1,1);
        }
    )";
    std::string tex_code_frag = R"(
        #version 330 core
        in vec4 f_color;
        in vec2 f_texCoord;
        out vec4 o_color;
        uniform sampler2D texU1;
        void main(){
            o_color = texture(texU1,f_texCoord) * f_color;
        }
    )";
    std::string text_code_vert = R"(
        #version 330 core
        layout(location=0) in vec2 v_pos;
        layout(location=1) in vec4 v_color;
        layout(location=2) in vec2 v_texCoord;
        out vec4 f_color;
        out vec2 f_texCoord;
        uniform mat4 view;
        void main(){
            f_color = v_color;
            f_texCoord = v_texCoord;
            gl_Position = view * vec4(v_pos,1,1);
        }
    )";
    std::string text_code_frag = R"(
        #version 330 core
        in vec4 f_color;
        in vec2 f_texCoord;
        out vec4 o_color;
        uniform sampler2D texture_U1;
        void main(){
            vec4 texColor = texture(texture_U1, f_texCoord);
            o_color = vec4(f_color.rgb,texColor.r*f_color.a);
        }
    )";
}