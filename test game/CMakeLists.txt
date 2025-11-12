find_package(LiteGL REQUIRED)
target_link_libraries(lite-game PUBLIC lite-api)
add_library(test SHARED testgame.cpp)
