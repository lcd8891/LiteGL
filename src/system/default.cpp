#include "priv_default.hpp"
#include "../window/window.hpp"

namespace LiteAPI{
    void setDefault(const LiteDefaults& _default){
        PRIV_default::default_settings = _default;
        PRIV_default::apply_defaults();
    }
}
namespace PRIV_default{
    void apply_defaults(){
        PRIV_Window::default_window_title = default_settings.window_title;
        PRIV_Window::default_default_size = default_settings.window_size;
        if(!default_settings.window_title.empty()){
            PRIV_Window::default_window_title = default_settings.window_title;
        }
        PRIV_Window::apply_defaults();
    }
}