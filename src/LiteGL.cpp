#include "window/window.hpp"
#include <exception>
#define LOGGER_GROUP "main"
#include <LiteGL/LiteGL.hpp>
#include "gameldr/gameldr.hpp"
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
void enable_ascii(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

void finalize(){
	
	LiteAPI::Logger::info("finalizing engine...");
	PRIV_Window::finalize();
}

void loop(){
	while(!PRIV_Window::isClosed()){
		PRIV_Window::pollEvents();
		LiteGame::on_frame();
		PRIV_Window::update();
	}
}

void start(){
	LiteAPI::Logger::info("LiteGL engine version "+std::to_string(LITEGL_VERSION_MAJOR)+"."+std::to_string(LITEGL_VERSION_MINOR));
	GameLDR::loadgame("./litegl-game");
	PRIV_Window::initialize();
	LiteAPI::Logger::info("window and context created.");
	try{
		LiteGame::on_initialize();
		loop();
	}catch(const std::exception& e){
		std::string err_msg;
		err_msg+="Internal error:\n";
		err_msg+=e.what();
		err_msg+="\n\nLiteGL Game: ";
		err_msg+=LiteGame::game_name;
		throw std::runtime_error(err_msg);
	}
	LiteGame::on_exit();
	finalize();
}


int main(int argc, char **argv) {
	enable_ascii();
	try{
		start();
	}catch(const std::exception &e){
		finalize();
		LiteAPI::Logger::error("Exception: "+std::string(e.what()));
		return 1;
	}
	return 0;
}
