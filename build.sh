#export EM_CACHE=~/.emscripten_cache
#export EM_CONFIG=$HOME/.emscripten

source ~/Programs/emcs/emsdk/emsdk_env.sh
emcc main.cpp -std=c++11 -s WASM=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -O3 -o index.js --preload-file assets/test.png --preload-file assets/font8x8.png
