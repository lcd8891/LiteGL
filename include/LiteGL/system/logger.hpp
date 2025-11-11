#pragma once
#include <string>
#include <sstream>

namespace LiteAPI{
    class Logger{
        private:
        const std::string _namespace;
        void flush(const std::string& message, const std::string& level, const std::string& color_code = "");

        public:
        class LogStream{
            private:
            Logger& _logger;
            std::stringstream _buffer;
            std::string _level;
            std::string _color;

        public:
            LogStream(Logger& logger, const std::string& level, const std::string& color = "");
            ~LogStream();
            
            template<typename T>
            LogStream& operator<<(const T& value) {
                _buffer << value;
                return *this;
            }
            LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
                _buffer << manip;
                return *this;
            }
        };
        Logger(std::string group);
        LogStream info();
        LogStream warn();
        LogStream error();
        
    };
    Logger* getLogger();
}