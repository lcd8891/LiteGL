#include <fstream>
#include <LiteGL/system/iniloader.hpp>
#include <sstream>
#include <algorithm>

namespace{
    void trim(std::string &str){
        const char* ws = " \t\n\r\f\v";
        size_t start = str.find_first_not_of(ws);
        size_t end = str.find_last_not_of(ws);
        str = (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }
    void remove_carriage_return(std::string &str) {
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    }
}

namespace LiteAPI{
    INISection::INISection(std::string _str){
        std::stringstream ss(_str);
        std::string line;
        while(std::getline(ss,line)){
            trim(line);
            if(line.empty() || line[0]==';')continue;
            size_t equal_ = line.find_first_of('=');
            if(equal_==std::string::npos)continue;
            std::string key = line.substr(0,equal_),value = line.substr(equal_+1);
            trim(key);trim(value);
            values[key]=value;
        }
    }
    std::string& INISection::operator[](std::string _key){
        return values[_key];
    }
    std::string INISection::operator[](std::string _key) const{
        auto it = values.find(_key);
        return (it!=values.end()) ? it->second : "";
    }
    const std::map<std::string,std::string>& INISection::getMap() const {
        return values;
    }

    INIFile::INIFile(std::string _str){
        std::stringstream ss(_str);
        std::string line,section,sectiondata;
        while(std::getline(ss,line)){
            if(line.empty())continue;
            remove_carriage_return(line);
            if(line[0]=='[' && line.back() == ']'){
                if(!sectiondata.empty()){
                    sections[section] = INISection(sectiondata);
                }
                section = line.substr(1,line.length()-2);
                trim(section);
                sectiondata.clear();
            }else if(line[0]==';') continue; 
            else if(!section.empty()) sectiondata += line + '\n';
        }
        if(!section.empty()) {
            sections[section] = INISection(sectiondata);
        }
    }
    INISection& INIFile::operator[](std::string _section){
        auto it = sections.find(_section);
        if(it == sections.end()){
            sections[_section] = INISection();
        }
        return sections[_section];
    }
    std::string INIFile::getINIString()const{
        std::stringstream ss;
        for(auto &it : sections){
            std::string section_name = it.first;
            ss << "[" << section_name << "]\n";
            const INISection& section = it.second;
            const std::map<std::string,std::string>& map = section.getMap();
            for(auto &it2 : map){
                ss << it2.first << "=" << it2.second << '\n';
            }
            ss << '\n';
        }
        return ss.str();
    }

    namespace INILoader{
        INIFile* loadFromRes(std::string _path){
            std::ifstream file("./res/configs/"+_path+".ini");
            if(!file.is_open()){
                throw std::runtime_error("Couldn't load ini file: ./res/configs/"+_path+".ini");
            }
            std::stringstream ss;
            ss << file.rdbuf();
            file.close();
            return new INIFile(ss.str());
        }
        void saveToRes(std::string _path,const INIFile* _file){
            std::string concha = _file->getINIString();
            std::ofstream file("./res/configs/"+_path+".ini");
            if(!file.is_open()){
                throw std::runtime_error("Couldn't save ini file: ./res/configs/"+_path+".ini");
            }
            file << concha;
            file.close();
        }
    }
}