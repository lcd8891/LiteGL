#include <LiteGL/glctrl/glctrl.hpp>
#include <GL/gl.h>
#include <LiteGL/types.hpp>

#define GL_ON_OFF(V) (_enable ? glEnable : glDisable)(V);

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
        void face_culling(bool _enable){
            GL_ON_OFF(GL_CULL_FACE)
        }
        void setCullFace(CullFace _face){
            glCullFace((int)_face);
        }
        void setFrontFace(FrontFace _face){
            glFrontFace((int)_face);
        }
        void setDepthBuffer(bool _enable){
            GL_ON_OFF(GL_DEPTH_TEST);
        }
    }
}