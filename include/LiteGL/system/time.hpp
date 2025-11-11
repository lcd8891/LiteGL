#pragma once
#include "types.hpp"
namespace LiteAPI{
    struct Time{
        uint8 hour;
        uint8 minute;
        uint8 second;
    };

    struct Date{
        uint16 year;
        uint8 month;
        uint8 day;
    };

    Time getCurrentTime();
    Date getCurrentDate();
}