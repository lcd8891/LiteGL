# First steps
You need to compile game lika a *SHARED* library. With CMAKE you can use `add_library(name SHARED name.cpp)`. You can link library by `lite-api` name.
And now in your main file you need add this strings:
```
#include <LiteGL/LiteGL.hpp>

_LITE_GAME_ std::string game_name = "GAME_NAME"; // Name of you game to indentificate. (It isn't offer to something)
_LITE_GAME_ void game_on_initialize(){}          // On LiteGL engine start.
_LITE_GAME_ void game_on_frame(){}               // On every window frame.
_LITE_GAME_ "C" void game_on_exit(){}            // On LiteGL engine end.
```

It is a 4 defines whats need to run your game. And if you run the game you get this messages (if cmd enabled).
```
[20:54:29] INFO: LiteGL engine version 1.0 by lcd8891
[20:54:29] INFO: loaded game: GAME_NAME
...
```

