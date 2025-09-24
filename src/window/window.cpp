#include <GL/glew.h>
#include "../../lib/stb_image.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "../privtypes.hpp"
#include <LiteGL/window/window.hpp>
#include <cstring>
#include <filesystem>

#define _MOUSE_BUTTONS 1024

GLFWwindow* window;
uint32 *_frames;
bool *_keys,_focused,__fullscreen;
uint32 	_current;
vector2<int16> _mouse_pos,_mouse_delta; 
int8 _mwheel_delta;
uint32 _char_input;

namespace PRIV_Window{
	vector2<uint16> window_size;
	vector2<uint16> default_default_size;
	std::string default_window_title;
	std::string default_window_icon;
	void initialize(){
		glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE,true);
        window = glfwCreateWindow(800,600,"LiteGL Engine",glfwGetPrimaryMonitor(),nullptr);
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
		window_size = {800,600};
		default_window_title = "LiteGL Engine";
		default_default_size = {800,600};
		default_window_icon = "ico.png";
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
		});
	}
	bool isClosed(){
		return glfwWindowShouldClose(window);
	}
	void update(){
		glfwSwapBuffers(window);
	}
	void finalize(){
		if(!glfwWindowShouldClose(window)){
			glfwSetWindowShouldClose(window,true);
		}
		glfwDestroyWindow(window);
		glfwTerminate();
		delete _frames;
		delete _keys;
	}
	void pollEvents(){
        _mouse_delta = {0,0};
        _mwheel_delta = 0;
		_char_input=0;
		_current++;
		glfwPollEvents();
	}
	void apply_defaults(){
		LiteAPI::Window::set_size(default_default_size);
		vector2<uint16> size = LiteAPI::Window::get_screen_size();
		size.x/=2;size.x -= (default_default_size.x / 2);
		size.y/=2;size.y -= (default_default_size.y / 2);
		LiteAPI::Window::set_position(size);
	}
	void set_window_icon(std::filesystem::path _path){
		std::filesystem::path full_path = "./res/";
		full_path/=_path;
		std::string str_path = full_path.string();
		int x,y,mode;
		unsigned char* data = stbi_load(str_path.c_str(),&x,&y,&mode,0);
		GLFWimage icon;icon.height=y,icon.width=x;icon.pixels=data;
		glfwSetWindowIcon(window,1,&icon);
		stbi_image_free(data);
	}
}

namespace LiteAPI{
	namespace Window{
		void setTitle(std::string _title){
			glfwSetWindowTitle(window,_title.c_str());
		}
		void setFullscreen(bool _fullscreen){
			__fullscreen = _fullscreen;
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);
			if(__fullscreen){
				glfwSetWindowMonitor(window,monitor,0,0,mode->width,mode->height,mode->refreshRate);
				PRIV_Window::window_size.x = mode->width;PRIV_Window::window_size.y = mode->height;
			}else{
				glfwSetWindowMonitor(window,nullptr,mode->width/2-400,mode->height/2-300,800,600,mode->refreshRate);
				PRIV_Window::window_size.x = 800;PRIV_Window::window_size.y = 600;
			}
			glViewport(0,0,PRIV_Window::window_size.x,PRIV_Window::window_size.y);
		}
		bool getFullscreen(){
			return __fullscreen;
		}
		vector2<uint16> getSize(){
			return PRIV_Window::window_size;
		}
		void set_position(vector2<uint16> _position){
			glfwSetWindowPos(window,_position.x,_position.y);
		}
		void set_size(vector2<uint16> _size){
			glfwSetWindowSize(window,_size.x,_size.y);
			glViewport(0,0,_size.x,_size.y);
			PRIV_Window::window_size = _size;
		}
		vector2<uint16> get_screen_size(){
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			return {mode->width,mode->height};
		}
		void set_attribute(LiteAPI::WindowAttribute _attr,int _value){
			glfwSetWindowAttrib(window,(int)_attr,_value);
		}
	}
	namespace Events{
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
