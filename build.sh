
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

	# patch the compiler, yes, i'm not fucking kidding
	sed -i 's/param ${ptrToString(param}/param ${ptrToString(param)}/g' upstream/emscripten/src/library_openal.js
	sed -i 's/No value for `${pEnumName}`/No value for ${pEnumName}/g' upstream/emscripten/src/library_openal.js
	sed -i 's/No value for `${name}`/No value for ${name}/g' upstream/emscripten/src/library_openal.js
	sed -i 's/${format}`;/${format}`);/g' upstream/emscripten/src/library_openal.js
	sed -i 's/${e}`;/${e}`);/g' upstream/emscripten/src/library_openal.js
	echo "${BOLD}OpenAL debug code patched!${NONE}"

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
emcc -sASSERTIONS -sRUNTIME_DEBUG -sOPENAL_DEBUG -lopenal -std=c++20 -Wno-c++17-extensions lib/source.cpp src/main.cpp -I./src -I./lib -I./lib/glm -I. -s WASM=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -O3 -o build/index.js --preload-file assets

rm -f -- build/index.html
cp index.html build/index.html
