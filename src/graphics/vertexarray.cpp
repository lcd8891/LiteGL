#include <LiteGL/graphics/vertexarray.hpp>
#include <memory>

#define VERIT VertexArray::VertexIterator

namespace LiteAPI{
    VERIT::VertexIterator(float* ptr, unsigned int vertex_size):current_ptr(ptr),step(vertex_size){}
    VERIT VERIT::operator++() {
        current_ptr += step;
        return *this;
    }
    VERIT VERIT::operator++(int) {
        VertexIterator temp = *this;
        current_ptr += step;
        return temp;
    }
    
    VERIT& VERIT::operator--() {
        current_ptr -= step;
        return *this;
    }
    VERIT VERIT::operator--(int) {
        VertexIterator temp = *this;
        current_ptr -= step;
        return temp;
    }
    VERIT VERIT::operator+(difference_type n) const {
        return VertexIterator(current_ptr + n * step, step);
    }
    VERIT VERIT::operator-(difference_type n) const {    
        return VertexIterator(current_ptr - n * step, step);
    }
    VERIT::difference_type VERIT::operator-(const VertexIterator& other) const {
        return (current_ptr - other.current_ptr) / step;
    }
    bool VERIT::operator==(const VertexIterator& other) const {
        return current_ptr == other.current_ptr;
    }
    bool VERIT::operator!=(const VertexIterator& other) const {
        return current_ptr != other.current_ptr;
    }
    bool VERIT::operator<(const VertexIterator& other) const {
        return current_ptr < other.current_ptr;
    }
    bool VERIT::operator>(const VertexIterator& other) const {
        return current_ptr > other.current_ptr;
    }
    bool VERIT::operator<=(const VertexIterator& other) const {
        return current_ptr <= other.current_ptr;
    }
    bool VERIT::operator>=(const VertexIterator& other) const {
        return current_ptr >= other.current_ptr;
    }
    float* VERIT::operator*() const {
        return current_ptr;
    }
    float* VERIT::operator->() const {
        return current_ptr;
    }
    float* VERIT::operator[](difference_type n) const {
        return current_ptr + n * step;
    }
    VERIT& VERIT::operator+=(difference_type n) {
        current_ptr += n * step;
        return *this;
    }
    VERIT& VERIT::operator-=(difference_type n) {
        current_ptr -= n * step;
        return *this;
    }
            
    VertexArray::VertexArray(unsigned vertex_size):vertex_size(vertex_size){}
    unsigned VertexArray::getVertexSize(){
        return vertex_size;
    }
    void VertexArray::insert(float* _data, unsigned vertex_count) {
        arr.insert(arr.end(), _data, _data + vertex_size * vertex_count);
    }
    void VertexArray::insert(float* _data, unsigned vertex_count,unsigned vertex_offset){
        arr.insert(arr.begin() + vertex_offset * vertex_size, _data, _data + vertex_size * vertex_count);
    }
    void VertexArray::replace(float value,unsigned attribute,unsigned from_vertex,unsigned vertex_count){
        if(attribute>=vertex_size)return;
        for(int i = 0;i< vertex_count;i++){
            arr[attribute + from_vertex * vertex_size] = value;
        }
    }
    void VertexArray::replace(float* value,unsigned from_vertex,unsigned size){
        std::copy(value,value + size * vertex_size,arr.begin() + from_vertex * vertex_size);
    }
    void VertexArray::erase(unsigned vertices,unsigned offset){
        arr.erase(arr.begin() + vertex_size * offset,arr.begin() + vertex_size * offset + vertex_size * vertices + 1);
    }
    float* VertexArray::getData(){
        return &arr[0];
    }
    void VertexArray::reserver(unsigned _vertices){
        arr.reserve(_vertices*vertex_size);
    }
    void VertexArray::clear(){
        arr.clear();
    }
    unsigned VertexArray::getVertexCount(){
        return arr.size() / vertex_size;
    }
    VertexArray::VertexIterator VertexArray::begin(){
        return VertexIterator(arr.data(), vertex_size);
    }
    VertexArray::VertexIterator VertexArray::end(){
        return VertexIterator(arr.data() + arr.size(), vertex_size);
    }
    VertexArray::VertexIterator VertexArray::operator[](unsigned vertex){
        return VertexIterator(arr.data() + vertex * vertex_size,vertex_size);
    }
}