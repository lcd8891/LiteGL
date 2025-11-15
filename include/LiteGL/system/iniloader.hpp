#pragma once
#include <string>
#include <map>

namespace LiteAPI{
    class INISection{
        private:
        std::map<std::string,std::string> values;
        public:
        INISection() = default;
        INISection(std::string _str);
        std::string& operator[](std::string _key);
        std::string operator[](std::string _key) const;
        const std::map<std::string,std::string>& getMap() const;
    };
    class INIFile{
        private:
        std::map<std::string,INISection> sections;
        public:
        INIFile(std::string _str);

        INISection& operator[](std::string _section);
        std::string getINIString() const;
    };
    namespace INILoader{
        INIFile* loadFromRes(std::string _path);
        void saveToRes(std::string _path,const INIFile* _file);
    }
}