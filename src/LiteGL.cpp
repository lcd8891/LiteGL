#include <exception>
#define LOGGER_GROUP "main"
#include <LiteGL/LiteGL.hpp>
#include <stdexcept>
#include <LiteGL/screen/screenmgr.hpp>
#include "gameldr/gameldr.hpp"
#include "window/window.hpp"

#include "system/priv_arguements.hpp"
#include "system/priv_logger.hpp"
#include "window/exception.hpp"
#include "system/signals.hpp"

namespace PRIV{
	namespace ScreenMGR{
        void initialize();
        void finalize();
	}
}

void finalize(){
	system_logger->info() << "finalizing engine...";
	PRIV::ScreenMGR::finalize();
	PRIV_Window::finalize();
	LiteAPI::ShaderBuffer::delete_all_shaders();
	LiteAPI::TextureBuffer::delete_all_textures();
	LiteAPI::ScreenBuffer::delete_all_screens();
	GameLDR::close();
}

void loop(){
	while(!PRIV_Window::isClosed()){
		PRIV_Window::pollEvents();
		LiteGame::on_frame();
		PRIV_Window::update();
	}
}

void start(){
	system_logger->info() << "LiteGL engine v"<<LITEGL_VERSION_MAJOR<<"."<<LITEGL_VERSION_MINOR<<", by lcd8891!";
	GameLDR::loadgame();
	PRIV_Window::initialize();
	system_logger->info() << "Window and events initialized...";
	try{
		Logger::init_for_game();
		PRIV::ScreenMGR::initialize();
		LiteGame::on_initialize();
		PRIV::Args::process_flags();
		system_logger->info() << "Initialize successfully!";
		loop();
		LiteGame::on_exit();
	}catch(const std::exception& e){
		std::string out = "RUNTIME ERROR:\n->Ingame execution error: game_id:"+LiteGame::game_name+"\n-->Reason: "+e.what();
		show_error(out);
	}
	finalize();
}


int main(int argc, char **argv) {
	PRIV::Args::parse_all(argc,argv);
	init_signal_handler(finalize);
	try{
		Logger::initialize();
		start();
	}catch(const std::exception &e){
		std::string out = "ENGINE RUNTIME ERROR:\n->Reason: ";
		out+=e.what();
		show_error(out);
		finalize();
		Logger::close();
		return 1;
	}
	Logger::close();
    #ifdef _WIN32
    PRIV::Args::hold_console();
    #endif
	return 0;
}
