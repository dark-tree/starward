
CYAN=`tput setaf 6`
BOLD=`tput bold`
NONE=`tput sgr0`

ROOT_DIR="$(pwd)"
BUILD_DIR="${ROOT_DIR}/build"

if [ ! -d "build" ]; then
	mkdir build
fi

# check if emscripten SDK is present
if [ ! -d "$BUILD_DIR/emsdk" ]; then
	printf "${BOLD}Emscripten SDK not present, installing...\n${NONE}"
	sleep 1

  cd "$BUILD_DIR" || exit
	mkdir emsdk
	cd emsdk

	git clone https://github.com/emscripten-core/emsdk.git .

	./emsdk install 3.1.54
	./emsdk activate 3.1.54
	echo "${BOLD}Emscripten SDK installed!${NONE}"

	# patch the compiler, yes, i'm not fucking kidding
	sed -i 's/param ${ptrToString(param}/param ${ptrToString(param)}/g' upstream/emscripten/src/library_openal.js
	sed -i 's/No value for `${pEnumName}`/No value for ${pEnumName}/g' upstream/emscripten/src/library_openal.js
	sed -i 's/No value for `${name}`/No value for ${name}/g' upstream/emscripten/src/library_openal.js
	sed -i 's/${format}`;/${format}`);/g' upstream/emscripten/src/library_openal.js
	sed -i 's/${e}`;/${e}`);/g' upstream/emscripten/src/library_openal.js
	echo "${BOLD}OpenAL debug code patched!${NONE}"

	echo
fi

# Load emscripten SDK
cd "${BUILD_DIR}" || exit
export EMSDK_QUIET=1
source ./emsdk/emsdk_env.sh

#cd "$ROOT_DIR" || exit
#emcc -sRUNTIME_DEBUG -sOPENAL_DEBUG -lopenal -std=c++20 -Wno-c++17-extensions $SOURCES -I./src -I./lib -I./lib/glm -I. -s WASM=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -O3 -o build/index.js --preload-file assets

cd "${ROOT_DIR}" || exit
emcmake cmake . -B build
cmake --build build/

# Copy index file
rm -f -- build/index.html
cp index.html build/index.html

