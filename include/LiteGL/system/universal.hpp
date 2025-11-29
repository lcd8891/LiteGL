#pragma once
#include <vector>

//Just for joke i made this
class Universal{
    private:
    void *data;

    public:
    Universal(void *data){
        this->data = data;
    }
    ~Universal(){
        delete static_cast<char*>(data);
    }


    template<typename T> T* getPtr(){
        return static_cast<T*>(data);
    }
    void setValue(void* value){
        delete static_cast<char*>(data);
        data = value;
    }
    template<typename T> T* getValue(){
        return static_cast<>
    }
};
using UArray = std::vector<Universal>;