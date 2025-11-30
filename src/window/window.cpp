#include <GL/glew.h>
#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../system/privtypes.hpp"
#include <LiteGL/window/window.hpp>
#include <cstring>
#include <filesystem>
#include <LiteGL/screen/screenmgr.hpp>
#include "../LiteData.hpp"
#include <functional>

#define _MOUSE_BUTTONS 1024

namespace PRIV{
	namespace ScreenMGR{
        void recalc_screenView();
		void recalc_screenView_noupdate();
	}
}

namespace Callbacks{
	std::function<void(bool)> focus = nullptr;
}

namespace{
	GLFWwindow* window;
	uint32 *_frames;
	bool *_keys,_focused,__fullscreen,_locked;
	uint32 	_current;
	vector2<int16> _mouse_pos,_mouse_delta; 
	int8 _mwheel_delta;
	uint32 _char_input;
	uint16 _fpsframe(0);
	uint16 _fps(0);
	double _prevTime(0);
	float _frameTime(0);
	const LiteAPI::INISection *window_config;
	vector2<uint16> default_size;
	vector2<uint16> get_from_string(std::string str){
		size_t pos = str.find('x');
		if(pos==std::string::npos)return {0,0};
		return { (uint16)std::stoi( str.substr(0,pos) ) , (uint16)std::stoi( str.substr(pos+1) ) };
	}
}

namespace PRIV_Window{
	vector2<uint16> window_size;
	void set_window_icon(std::filesystem::path _path){
		std::string str_path = _path.string();
		int x,y,mode;
		unsigned char* data = stbi_load(str_path.c_str(),&x,&y,&mode,0);
		GLFWimage icon;icon.height=y,icon.width=x;icon.pixels=data;
		glfwSetWindowIcon(window,1,&icon);
		stbi_image_free(data);
	}
	void initialize(){
		glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE,true);
		window_config = &(*LiteDATA::main_config)["window"];
		default_size = get_from_string((*window_config)["size"]);
		window_size = default_size;
        window = glfwCreateWindow(default_size.x,default_size.y,(*window_config)["title"].c_str(),nullptr,nullptr);
		if(!window){
			glfwTerminate();
			throw std::runtime_error("Couldn't create GLFW window.");
		}
		glfwMakeContextCurrent(window);
		glewExperimental = true;
		if(glewInit() != GLEW_OK){
			glfwTerminate();
			throw std::runtime_error("Couldn't initialize glew.");
		}
		glViewport(0,0,window_size.x,window_size.y);
		glfwSetWindowSizeLimits(window,800,600,-1,-1);

		
        _frames = new uint32[1032];
        _keys = new bool[1032];

        memset(_frames,0,sizeof(uint32)*1032);
        memset(_keys,false,sizeof(bool)*1032);

		glfwSetWindowSizeCallback(window,[](GLFWwindow *window,int width,int height){
			PRIV_Window::window_size.x = width;
			PRIV_Window::window_size.y = height;
			glViewport(0,0,width,height);
			PRIV::ScreenMGR::recalc_screenView();
		});
		glfwSetCharCallback(window,[](GLFWwindow* window,uint32 ch){
			_char_input = ch;
		});
		glfwSetCursorPosCallback(window,[](GLFWwindow* window,double xpos,double ypos){
			_mouse_delta.x+=xpos-_mouse_pos.x;
			_mouse_delta.y+=ypos-_mouse_pos.y;
			_mouse_pos.x=xpos;
			_mouse_pos.y=ypos;
		});
		glfwSetKeyCallback(window,[](GLFWwindow* window,int key, int scancode, int action, int mode){
			if(action == GLFW_PRESS){
				_keys[key]=true;
				_frames[key]=_current;
			}else if(action==GLFW_RELEASE){
				_keys[key]=false;
				_frames[key]=_current;
			}
		});
		glfwSetMouseButtonCallback(window,[](GLFWwindow* window, int button, int action, int mode){
			if (action == GLFW_PRESS){
				_keys[_MOUSE_BUTTONS+button] = true;
				_frames[_MOUSE_BUTTONS+button] = _current;
			}
			else if (action == GLFW_RELEASE){
				_keys[_MOUSE_BUTTONS+button] = false;
				_frames[_MOUSE_BUTTONS+button] = _current;
			}
		});
		glfwSetScrollCallback(window,[](GLFWwindow* window,double xoffset,double yoffset){
			_mwheel_delta = yoffset;
		});
		glfwSetWindowFocusCallback(window,[](GLFWwindow *window,int focused){
			_focused = focused;
			if(Callbacks::focus) Callbacks::focus(_focused);
			if(_locked){
				LiteAPI::Window::setMouseLock(false);
			}
		});
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		_fps = glfwGetVideoMode(monitor)->refreshRate;
		_prevTime = glfwGetTime();
		_frameTime = 1.f/_fps;
		PRIV::ScreenMGR::recalc_screenView_noupdate();
		PRIV_Window::set_window_icon("res/icon.png");
	}
	bool isClosed(){
		return glfwWindowShouldClose(window);
	}
	void update(){
		glfwSwapBuffers(window);
		double current = glfwGetTime();
		if(current - _prevTime >= 1){
			_fps=_fpsframe;
			_fpsframe=0;
			_prevTime=current;
			_frameTime=1.f/_fps;
		}else{
			_fpsframe++;
		}
	}
	void finalize(){
		if(!window)return;
		if(!glfwWindowShouldClose(window)){
			glfwSetWindowShouldClose(window,true);
		}
		glfwDestroyWindow(window);
		glfwTerminate();
		delete[] _frames;
		delete[] _keys;
	}
	void pollEvents(){
        _mouse_delta = {0,0};
        _mwheel_delta = 0;
		_char_input=0;
		_current++;
		glfwPollEvents();
	}
}

namespace LiteAPI{
	namespace Window{
		void setTitle(std::string _title){
			glfwSetWindowTitle(window,_title.c_str());
		}
		void setFullscreen(bool _fullscreen, vector2<uint16> _size){
			__fullscreen = _fullscreen;
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);
			vector2<uint16> nullpos = {0,0};
			if(__fullscreen){
				if(_size.is_equals(nullpos)){
					glfwSetWindowMonitor(window,monitor,0,0,mode->width,mode->height,mode->refreshRate);
					PRIV_Window::window_size.x = mode->width;PRIV_Window::window_size.y = mode->height;
				}else{
					glfwSetWindowMonitor(window,monitor,0,0,_size.x,_size.y,mode->refreshRate);
					PRIV_Window::window_size.x = _size.x;PRIV_Window::window_size.y = _size.y;
				}
			}else{
				PRIV_Window::window_size = default_size;
				glfwSetWindowMonitor(window,nullptr,
					mode->width/2-PRIV_Window::window_size.x/2,
					mode->height/2-PRIV_Window::window_size.y/2,
					PRIV_Window::window_size.x,PRIV_Window::window_size.y,
					mode->refreshRate);
			}
			glViewport(0,0,PRIV_Window::window_size.x,PRIV_Window::window_size.y);
			PRIV::ScreenMGR::recalc_screenView();
		}
		bool getFullscreen(){
			return __fullscreen;
		}
		vector2<uint16> getSize(){
			return PRIV_Window::window_size;
		}
		void setPosition(vector2<uint16> _position){
			glfwSetWindowPos(window,_position.x,_position.y);
		}
		void setSize(vector2<uint16> _size){
			glfwSetWindowSize(window,_size.x,_size.y);
			glViewport(0,0,_size.x,_size.y);
			PRIV_Window::window_size = _size;
			PRIV::ScreenMGR::recalc_screenView();
		}
		vector2<uint16> getScreenSize(){
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			return {(uint16)mode->width,(uint16)mode->height};
		}
		void setAttribute(LiteAPI::WindowAttribute _attr,int _value){
			glfwSetWindowAttrib(window,(int)_attr,_value);
		}
		uint16 getFPS(){
			return _fps;
		}
		const float& getFrameTime(){
			return _frameTime;
		}
		void setMouseLock(bool lock){
			_locked = lock;
			glfwSetInputMode(window,GLFW_CURSOR,(lock) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}
		const bool& getMouseLock(){
			return _locked;
		}
		void close(){
			glfwSetWindowShouldClose(window,true);
		}
	}
	namespace Events{
		void setWindowFocusEvent(std::function<void(bool)> _func){
			Callbacks::focus = _func;
		}
		const vector2<int16>& getMousePosition(){
			return _mouse_pos;
		}
		const vector2<int16>& getMouseDelta(){
			return _mouse_delta;
		}
		uint32 getInputChar(){
			return _char_input;
		}
		uint8 getScroll(){
			return _mwheel_delta;
		}
		bool getFocused(){
			return _focused;
		}
		bool mouse(MButton _button){
			int index = _MOUSE_BUTTONS+(int)_button;
			return _keys[index];
		}
		bool mousep(MButton _button){
			int index = _MOUSE_BUTTONS+(int)_button;
			return _keys[index] && _frames[index] == _current;
		}
		bool key(KBKey _key){
        	return _keys[(int)_key];
		}
		bool keyp(KBKey _key){
        	return _keys[(int)_key] && _frames[(int)_key] == _current;
		} 
	}
}
