#pragma once

#include <filesystem>
#include <string>
#include <functional>

class DLLObject{
    private:
    void* handle = nullptr;
    std::filesystem::path path;

    void* getSymbol(const std::string &name) const ;
    public:
    DLLObject(std::filesystem::path _pt);
    ~DLLObject();

    DLLObject(const DLLObject&) = delete;
    DLLObject& operator=(const DLLObject&) = delete;

    template<typename T>
    void* getObject(const std::string &name) const {
        void* s = getSymbol(name);
        if(!s){
            throw std::runtime_error(path.string()+" - coudn't load symbol: "+name);
        }
        return reinterpret_cast<T*>(s)
    }
    template<typename T>
    std::function<T> getFunction(const std::string &name) const {
        void* s = getSymbol(name);
        if(!s){
            throw std::runtime_error(path.string()+" - coudn't load symbol: "+name);
        }
        return reinterpret_cast<T>(s);
    }
    void reload();
};
