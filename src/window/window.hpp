#pragma once
#include <LiteGL/window/window.hpp>

namespace PRIV_Window{
	void initialize();
	bool isClosed();
	void update();
	void finalize();
	void pollEvents();
}
