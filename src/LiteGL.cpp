#include <exception>
#define LOGGER_GROUP "main"
#include <LiteGL/LiteGL.hpp>
#include <stdexcept>
#include <LiteGL/screen/screenmgr.hpp>
#include "window/window.hpp"

#include "system/priv_cache.hpp"
#include "system/priv_arguements.hpp"
#include "system/priv_logger.hpp"
#include "system/dllobject.hpp"
#include "window/exception.hpp"
#include "system/signals.hpp"
#include "graphics/fontloader.hpp"
#include "LiteData.hpp"

namespace PRIV{
	namespace ScreenMGR{
        void initialize();
        void finalize();
	}
}

void finalize(){
	system_logger->info() << "finalizing engine...";
	PRIV::ScreenMGR::finalize();
	PRIV::FontLoader::close();
	PRIV_Window::finalize();
	LiteAPI::ScreenAssets::deleteAll();
	LiteAPI::TextureAssets::deleteAll();
	LiteAPI::ScreenAssets::deleteAll();
}

void loop(){
	while(!PRIV_Window::isClosed()){
		PRIV_Window::pollEvents();
		PRIV_Window::update();
	}
}
void base_init(){
	system_logger->info() << "LiteGL engine v"<<LITEGL_VERSION_MAJOR<<"."<<LITEGL_VERSION_MINOR<<", by lcd8891!";
	LiteDATA::main_config = LiteAPI::INILoader::loadFromRes("engine");
	system_logger->info() << "Loaded engine config...";
	Cache::check_directories();
	PRIV_Window::initialize();
	system_logger->info() << "Window and events initialized...";
	PRIV::ScreenMGR::initialize();
	system_logger->info() << "Engine config loaded...";
	PRIV::FontLoader::loadfrom("./res/font");
	system_logger->info() << "Font initialized...";
}

void start(){
	base_init();
	try{
		Logger::init_for_game();
		system_logger->info() << "Created game logger...";
		system_logger->info() << "begin";

		PRIV::Args::process_flags();
		system_logger->info() << "Initialize successfully!";
		loop();

	}catch(const std::exception& e){
		std::string out = "RUNTIME ERROR:\n->Ingame execution error: game_id:"+"\n-->Reason: "+e.what();
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
