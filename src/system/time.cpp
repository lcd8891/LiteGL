#include <LiteGL/system/time.hpp>
#include <ctime>
#include <LiteGL/system/types.hpp>

namespace LiteAPI{
    Time getCurrentTime(){
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        return {(uint8)localTime->tm_hour,(uint8)localTime->tm_min,(uint8)localTime->tm_sec};
    }
    Date getCurrentDate(){
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        return {(uint16)(localTime->tm_year) + (uint16)1900,(uint8)localTime->tm_mon + (uint8)1,(uint8)localTime->tm_mday};
    }
}
