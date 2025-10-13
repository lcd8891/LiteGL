# Рисуем треугольник
Для начала нужно сделать шейдер. Для этого есть 2 компонента LiteAPI - `ShaderConstructor` и `ShaderBuffer`. **ShaderConstructor** - это конструктор шейдеров, который способен соеденять и компилировать код шейдеров в 1 программу. **ShaderBuffer** - буффер для хранения шейдеров в памяти движка. Сами шейдеры находятся в *res/shaders* в любом формате.

## Создание шейдера
Для работы программы нужно 2 основных шейдера OpenGL: вершинный и фрагментный 
### Код вершинного шейдера (shader.vert)
```
#version 330 core
layout (location = 0) in vec3 v_pos;
void main(){
    gl_Position = vec4(v_pos,1);
}
```
### Код фрагментного шейдера (shader.frag)
```
#version 330 core
out vec4 f_color;
void main(){
    f_color = vec4(1,1,1,1);
}
```
### Загрузка шейдера
Теперь нужно создать объект ShaderConstructor:
```
LiteAPI::ShaderConstructor constructor;                                 // Объект конструктор шейдера
constructor.addFromFile("shader.vert",LiteAPI::ShaderType::Vertex);     // Добавление шейдера типа Vertex
constructor.addFromFile("shader.frag",LiteAPI::ShaderType::Fragment);   // Добавление шейдера типа Fragment
```
Следующим шагом нужно добавить в буффер и сохранить его в объект Shader: `LiteAPI::Shader* shader = LiteAPI::ShaderBuffer::load_from_constructor(constructor,"main");`. Чтобы использовать шейдер нужно использовать `shader->bind();`.
## Создание меша
Меш хранит данные о вершинах, которые затем можно отрисовать. Для его создания нужно 3 параметра: float массив вершин, int массив аттрибутов, число вершин.
### Код меша
```
const float vertex_buffer[] = {0,0,0,1,1,1 ,0,1,0,1,1,1, 1,0,0,1,1,1}; // Массив вершин
const int attributes[] = {3,3};                                        // Массив аттрибутов
LiteAPI::Mesh* mesh = new LiteAPI::Mesh(vertex_buffer,3,attributes);   // Создание меша
```
Теперь можно отрисовавть меш функцией **draw(ПРИМИТИВ)**: `mesh->draw(LiteAPI::Primitive::Tringles);`.
## Советы
Буфферы автоматически удаляют память и очищаются.