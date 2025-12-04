#include "dllobject.hpp"
#include <filesystem>
#include "../system/priv_logger.hpp"
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define LIB_FORMAT ".dll"
#elif __linux__
#include <dlfcn.h>
#define LIB_FORMAT ".so"
#endif


namespace LiteGame{
    std::string game_name;
    void (*on_initialize)();
    void (*on_frame)();
    void (*on_exit)();
}

std::filesystem::path path;

#ifdef WIN32
DLLObject::DLLObject(std::filesystem::path _pt):path(_pt){
    std::wstring widePath = toPlatformString(libPath);
    HMODULE hModule = LoadLibraryW(widePath.c_str());
    handle = hModule;
}
DLLObject::~DLLObject(){
    HMODULE hModule = static_cast<HMODULE>(handle);
    FreeLibrary(hModule);
}
void* DLLObject::getSymbol(const std::string &name) const {
    HMODULE hModule = static_cast<HMODULE>(handle);
    FARPROC proc = GetProcAddress(hModule, symbolName.c_str());
    return reinterpret_cast<void*>(proc);
}
void DLLObject::reload(){
    HMODULE hModule = static_cast<HMODULE>(handle);
    FreeLibrary(hModule);
}
#else
DLLObject::DLLObject(std::filesystem::path _pt):path(_pt){
    handle = dlopen(path.c_str(),RTLD_NOW | RTLD_GLOBAL);
    if(!handle){
        throw std::runtime_error("couldn't load dynamic object: "+path.string()+"\nReason: "+dlerror());
    }
}
DLLObject::~DLLObject(){
    dlclose(handle);
}
void* DLLObject::getSymbol(const std::string &name) const {
    return dlsym(handle,name.c_str());
}
void DLLObject::reload(){
    dlclose(handle);
    handle = dlopen(path.c_str(),RTLD_NOW | RTLD_GLOBAL);
    if(!handle){
        throw std::runtime_error("couldn't load dynamic object: "+path.string()+"\nReason: "+dlerror());
    }
}
#endif