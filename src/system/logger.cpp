#include "priv_logger.hpp"
#include "../gameldr/gameldr.hpp"
#include <LiteGL/system/logger.hpp>
#include <LiteGL/system/time.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>

#define LOG_FILES_MAX 10

namespace{
    LiteAPI::Logger* game_logger;
    std::ofstream log_file;
}

    LiteAPI::Logger *system_logger;
    namespace Logger{
        void initialize(){
            system_logger = new LiteAPI::Logger("litegl");
        }
        void open_in_file(){
            log_file.open("litegl.log", std::ios::app);
        }
        void init_for_game(){
            game_logger = new LiteAPI::Logger(LiteGame::game_name);
        }
        void close(){
            if(log_file.is_open())log_file.close();
        }
    }

namespace LiteAPI{
    Logger* getLogger(){
        return game_logger;
    }

    Logger::LogStream::LogStream(Logger& logger, const std::string& level, const std::string& color)
    : _logger(logger), _level(level), _color(color) {}
    Logger::LogStream::~LogStream() {
        _logger.flush(_buffer.str(), _level, _color);
    }

    Logger::Logger(std::string group):_namespace(group){}
    Logger::LogStream Logger::info() {
        return LogStream(*this, "INFO");
    }

    Logger::LogStream Logger::warn() {
        return LogStream(*this, "WARNING", "\e[0;33m");
    }

    Logger::LogStream Logger::error() {
        return LogStream(*this, "ERROR", "\e[0;31m");
    }
    void Logger::flush(const std::string& message, const std::string& level, const std::string& color) {
        Time t = getCurrentTime();
        std::stringstream ss;
        
        ss << std::setfill('0');
        ss << "[" << std::setw(2) << +t.hour;
        ss << ":" << std::setw(2) << +t.minute;
        ss << ":" << std::setw(2) << +t.second << "]";
        
        ss << " ( " << _namespace << " | ";
        if (!color.empty()) {
            ss << color << level << "\e[0m";
        } else {
            ss << level;
        }
        ss << " ) " << message;
        
        std::string log_entry = ss.str();
        
        if (level == "ERROR") {
            std::cerr << log_entry << std::endl;
        } else {
            std::cout << log_entry << std::endl;
        }
        
        if (log_file.is_open()) {
            std::string file_entry = log_entry;
            size_t pos = 0;
            while ((pos = file_entry.find("\e[0;33m")) != std::string::npos) {
                file_entry.replace(pos, 7, "");
            }
            while ((pos = file_entry.find("\e[0;31m")) != std::string::npos) {
                file_entry.replace(pos, 7, "");
            }
            while ((pos = file_entry.find("\e[0m")) != std::string::npos) {
                file_entry.replace(pos, 4, "");
            }
            
            log_file << file_entry << std::endl;
            log_file.flush();
        }
    }
}   