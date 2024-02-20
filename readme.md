## Test Game
WebGL game written in C++

#### Build For Web
To build run the `build.sh` script, then start a python server
with `server.sh` and open `http://localhost:8080/` in a browser.

#### Build For Linux
This is still WIP, run `g++ -std=c++20 lib/source.cpp lib/winx/winx.c lib/glad/glad.c src/game/*.cpp src/platform.cpp src/main.cpp -I./src -I./lib -I./lib/glm -I. -ldl -lGL -lX11 -lXcursor -lopenal -o build/main && ./build/main`
