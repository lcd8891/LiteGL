#pragma once
#include <LiteGL/system/types.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <string>
#include "screenitems.hpp"
#include <functional>

namespace LiteAPI{
    class ScreenItem;
    class Screen{
        private:
        std::vector<std::pair<std::string,ScreenItem*>> items;
        
        public:
        ~Screen();
        void addItem(std::string _name,ScreenItem *_item);
        void update();
        void parseData();
        void updateRelatived();
        ScreenItem* getItem(std::string _name);
    };
    namespace ScreenMGR{
        extern const glm::mat4 &screenView;
        void setScreen(std::string _name);
        void updateScreen();
        void renderScreen();
        Screen* getCurrentScreen();
    }
}