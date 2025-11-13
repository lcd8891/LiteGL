#pragma once
#include <LiteGL/system/types.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <string>
#include "screenitems.hpp"

namespace LiteAPI{
    class ScreenItem;
    class Screen{
        private:
        std::unordered_map<std::string,ScreenItem*> items;
        
        public:
        ~Screen();
        void operator()();
        void add_item(std::string _name,ScreenItem *_item);
        void update();
        ScreenItem* getItem(std::string _name);
    };
    namespace ScreenMGR{
        extern const glm::mat4 &screenView;
        void set_screen(std::string _name);
        void update_screen();
        void render_screen();
    }
}