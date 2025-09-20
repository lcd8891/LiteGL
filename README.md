# LiteGL
LiteGL is a engine for easy creating 2d and 3d games on OpenGL
**Warning: to compile library read a requirements!**

```
#define LITEGL_VERSION_MAJOR 1
#define LITEGL_VERSION_MINOR 0
```
version 1.0 

## How it work?
Engine executable file run game from litegl-game.dll (**Windows**) or litegl-game.so (**linux**). Game can access with engine by LiteAPI, what give you a buffers to storage resources, OpenGL objects, window control.

## Problems (solved and need to solve)
1. idk how to link game with LiteAPI using CMAKE (i forgot how to do it)
2. no template for game

## Requirement (now)
|Library|Version|
|:-----:|:-----:|
|**C++**|**17+**|
|GLFW|3.4-1|
|GLEW|2.2.0-3|
|GLM|1.0.1-1|
|Freetype 2|2.13.3-1|

## Documentation 
You can read documentation in docs folder. *(maybe)*
