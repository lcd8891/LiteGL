#pragma once
#include <LiteGL/window/window.hpp>
#include <filesystem>

namespace PRIV_Window{
	extern vector2<uint16> window_size;
	extern vector2<uint16> default_default_size;
	extern std::string default_window_title;
	extern std::string default_window_icon;
	void initialize();
	bool isClosed();
	void update();
	void finalize();
	void pollEvents();
	void apply_defaults();
	void set_window_icon(std::filesystem::path _path);
}
