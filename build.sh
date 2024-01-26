
CYAN=`tput setaf 6`
BOLD=`tput bold`
NONE=`tput sgr0`

ROOT_DIR=`pwd`
BUILD_DIR="$ROOT_DIR/build"

if [ ! -d "build" ]; then
	mkdir build
fi

# check if emscripten SDK is present
cd "$BUILD_DIR"
if [ ! -d "emsdk" ]; then
	printf "${BOLD}Emscripten SDK not present, installing...\n${NONE}"
	sleep 1

	mkdir emsdk
	cd emsdk

	git clone https://github.com/emscripten-core/emsdk.git .
	./emsdk install latest
	./emsdk activate latest
	echo
fi

# check if library lock file is present
cd "$BUILD_DIR"
if [ ! -f "deps.lock" ]; then
	printf "${BOLD}Synchronizing dependencies, this can take some time...\n${NONE}"
	sleep 1

	touch deps.lock

	cd "$ROOT_DIR/lib"
	source ./git-sync-deps
	echo
fi

# load emscripten SDK
cd "$BUILD_DIR"
source ./emsdk/emsdk_env.sh

# TODO: move to cmake (?)
cd "$ROOT_DIR"
emcc -std=c++20 -Wno-c++17-extensions src/main.cpp -I./src -I./lib -I./lib/glm -I. -std=c++11 -s WASM=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -O3 -o build/index.js --preload-file assets

rm -f -- build/index.html
cp index.html build/index.html
