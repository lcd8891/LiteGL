#include <LiteGL/graphics/mesh.hpp>
#include <LiteGL/graphics/vertexarray.hpp>
#include <GL/glew.h>

LiteAPI::Mesh::Mesh(const float* _buffer,uint64 _vertices,const int* _attr):vertices(_vertices){
    vertex_size = 0;
    for(int i = 0;_attr[i];i++){
        vertex_size += _attr[i];
    }
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);

    glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_size*vertices, _buffer,GL_STATIC_DRAW);

    int offset = 0;
    for(int i = 0;_attr[i];i++){
        int size = _attr[i];
	    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size*sizeof(float), (GLvoid*)(offset*sizeof(float)));
	    glEnableVertexAttribArray(i);
        offset += _attr[i];
    }

	glBindVertexArray(0);
}
LiteAPI::Mesh::Mesh(const VertexArray* array, const int* _attr){
    vertex_size = array->getVertexSize();
    const float* _buffer = array->getData();
    vertices = array->getVertexCount();
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);

    glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_size*vertices, _buffer,GL_STATIC_DRAW);

    int offset = 0;
    for(int i = 0;_attr[i];i++){
        int size = _attr[i];
	    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size*sizeof(float), (GLvoid*)(offset*sizeof(float)));
	    glEnableVertexAttribArray(i);
        offset += _attr[i];
    }

	glBindVertexArray(0);
}
LiteAPI::Mesh::~Mesh(){
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(1,&vbo);
}
void LiteAPI::Mesh::reload(const float* _buffer,uint64 _vertices){
	this->vertices = _vertices;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * _vertices, _buffer, GL_STATIC_DRAW);
}
void LiteAPI::Mesh::reload(const VertexArray* array){
	this->vertices = array->getVertexCount();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, array->getData(), GL_STATIC_DRAW);
}
void LiteAPI::Mesh::draw(Primitive _primitive){
    glBindVertexArray(vao);
    glDrawArrays((unsigned int)(_primitive),0,vertices);
    glBindVertexArray(0);
}
void LiteAPI::Mesh::drawPart(Primitive _primitive,uint64 _vertices,uint64 _offset){
    glBindVertexArray(vao);
    glDrawArrays((unsigned int)(_primitive),_offset,_vertices);
    glBindVertexArray(0);
}


LiteAPI::DynamicMesh::DynamicMesh(const float* _buffer,uint64 _vertices,const int* _attr):vertices(_vertices){
    vertex_size = 0;
    for(int i = 0;_attr[i];i++){
        vertex_size += _attr[i];
    }
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);

    glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_size*_vertices, _buffer,GL_DYNAMIC_DRAW);

    int offset = 0;
    for(int i = 0;_attr[i];i++){
        int size = _attr[i];
	    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size*sizeof(float), (GLvoid*)(offset*sizeof(float)));
	    glEnableVertexAttribArray(i);
        offset += _attr[i];
    }
    
	glBindVertexArray(0);
}
LiteAPI::DynamicMesh::DynamicMesh(const VertexArray* array, const int* _attr){
    vertex_size = array->getVertexSize();
    const float* _buffer = array->getData();uint64 _vertices = array->getVertexCount();
    
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);

    glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_size*_vertices, _buffer,GL_DYNAMIC_DRAW);

    int offset = 0;
    for(int i = 0;_attr[i];i++){
        int size = _attr[i];
	    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size*sizeof(float), (GLvoid*)(offset*sizeof(float)));
	    glEnableVertexAttribArray(i);
        offset += _attr[i];
    }

	glBindVertexArray(0);
}
LiteAPI::DynamicMesh::~DynamicMesh(){
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(1,&vbo);
}
void LiteAPI::DynamicMesh::reload(const float* _buffer,uint64 _vertices){
    this->vertices = _vertices;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * _vertices, _buffer, GL_DYNAMIC_DRAW);
}
void LiteAPI::DynamicMesh::reload(const VertexArray* array){
    this->vertices = array->getVertexCount();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, array->getData(), GL_DYNAMIC_DRAW);
}
void LiteAPI::DynamicMesh::reloadPart(const float* _buffer,uint64 _vertices,uint64 _offset){
	this->vertices = _vertices;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * _offset, _vertices * vertex_size * sizeof(float), _buffer);
}
void LiteAPI::DynamicMesh::reloadPart(const VertexArray* array,uint64 _offset){
	this->vertices = array->getVertexCount();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * _offset, vertices * vertex_size * sizeof(float), array->getData());
}
void LiteAPI::DynamicMesh::draw(Primitive _primitive){
    glBindVertexArray(vao);
    glDrawArrays((unsigned int)(_primitive),0,vertices);
    glBindVertexArray(0);
}
void LiteAPI::DynamicMesh::drawPart(Primitive _primitive,uint64 _vertices,uint64 _offset){
    glBindVertexArray(vao);
    glDrawArrays((unsigned int)(_primitive),_offset,_vertices);
    glBindVertexArray(0);
}