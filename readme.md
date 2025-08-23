## Test Game
WebGL game written in C++

#### Build For Web
To build run the `build.sh` script, then start a python server
with `server.sh` and open `http://localhost:8080/` in a browser.

#### Build For Linux
To build run the cmake script in the root directory,
then start the executable with `./build-native/main`.

```bash
cmake . -B build-native
cmake --build build-native

# Run native build
./build-native/main
```
