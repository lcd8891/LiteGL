#include <LiteGL/time.hpp>
#include <ctime>

namespace LiteAPI{
    Time getCurrentTime(){
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        return {localTime->tm_hour,localTime->tm_min,localTime->tm_sec};
    }
    Date getCurrentDate(){
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        return {localTime->tm_year + 1900,localTime->tm_mon + 1,localTime->tm_mday};
    }
}