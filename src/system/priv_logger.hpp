#pragma once
#include <LiteGL/system/logger.hpp>
#include "dllobject.hpp"

extern LiteAPI::Logger *system_logger;
namespace Logger{
    void initialize();
    void open_in_file();
    void init_for_game(const DLLObject* game);
    void close();
}