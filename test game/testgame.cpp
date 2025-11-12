#include <LiteGL/LiteGL.hpp>

_LITE_GAME_ std::string game_name = "tornado_chaser";

LiteAPI::Mesh* mesh;
LiteAPI::Shader* shader;
LiteAPI::Logger* logger = nullptr;

_LITE_GAME_ void game_on_initialize(){
    logger = LiteAPI::getLogger();
    LiteAPI::LiteDefaults defaults;defaults.icon_path="icon.png";defaults.window_size={1280,720};defaults.window_title="Tornado Chaser";
    LiteAPI::setDefault(defaults);
    LiteAPI::ShaderConstructor constructor;
    constructor.addFromFile("shader.vert",LiteAPI::ShaderType::Vertex);
    constructor.addFromFile("shader.frag",LiteAPI::ShaderType::Fragment);
    shader = LiteAPI::ShaderBuffer::load_from_constructor(constructor,"main");
    LiteAPI::GLCtrl::clear_color({25,25,25,25});
    const float verbuffer[] = {0,0,0,1,1,1 ,0,1,0,1,1,1, 1,0,0,1,1,1};
    const int buf[] = {3,3};
    mesh = new LiteAPI::Mesh(verbuffer,3,buf);
    logger->info() << "Hello world!";
}

_LITE_GAME_ void game_on_frame(){
    LiteAPI::GLCtrl::clear();
    shader->bind();
    mesh->draw(LiteAPI::Primitive::Triangles);
}

_LITE_GAME_ void game_on_exit(){

}