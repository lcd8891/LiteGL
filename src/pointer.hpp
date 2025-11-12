#pragma once

template<typename T>
class Pointer{
    private:
    T* ptr;
    public:
    Pointer():ptr(new T){}
    Pointer(const T &value):ptr(new T(value)){}
    ~Pointer(){delete ptr;}
    T get_data(){return *ptr;}
    void operator(const T& value){*ptr = value;}
};