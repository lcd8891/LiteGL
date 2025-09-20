#include <LiteGL/glctrl/glctrl.hpp>
#include <GL/gl.h>
#include <LiteGL/types.hpp>

namespace LiteAPI{
    namespace GLCtrl{
        void clear(bool color_buffer,bool depth_buffer){
            unsigned int b;
            if(color_buffer && depth_buffer){
                b = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
            }else if(color_buffer || depth_buffer){
                b = (color_buffer) ? GL_COLOR_BUFFER_BIT : GL_DEPTH_BUFFER_BIT;
            }else{
                b = 0;
            }
            glClear(b);
        }
        void clear_color(color _color){
            glClearColor(_color.r / 255.f, _color.g / 255.f,_color.b / 255.f,_color.a / 255.f);
        }
    }
}