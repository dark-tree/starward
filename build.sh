
CYAN=`tput setaf 6`
BOLD=`tput bold`
NONE=`tput sgr0`

ROOT_DIR=`pwd`
BUILD_DIR="$ROOT_DIR/build"

TARGET="web"
RUN="false"

while [[ $# -gt 0 ]]; do
	case "$1" in
		--clean)
			if [ -d "build" ]; then
				rm -rf build
			fi
			shift
			;;
		--native)
			TARGET="native"
			shift
			;;
		--web)
			TARGET="web"
			shift
			;;
	  --run)
	    RUN="true"
	    shift
	    ;;
		--help)
			echo "C/C++ Builder"
			echo "Usage: script.sh [options]"
			echo "Options:"
			echo "  --clean           (re-)generate build system"
			echo "  --native          Build for native linux"
			echo "  --web             Build for emscripten"
			echo "  --run             Execute the native executable"
			echo "  --help            display this help message"
			exit
			;;
		*)
			shift
			;;
	esac
done

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

	./emsdk install 3.1.54
	./emsdk activate 3.1.54

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

SOURCES="lib/source.cpp src/game/*.cpp src/game/entity/*.cpp src/main.cpp"

if [ "$TARGET" = "web" ]; then
	# load emscripten SDK
	cd "$BUILD_DIR"
	export EMSDK_QUIET=1
	source ./emsdk/emsdk_env.sh

	cd "$ROOT_DIR"
	emcc -sRUNTIME_DEBUG -sOPENAL_DEBUG -lopenal -std=c++20 -Wno-c++17-extensions $SOURCES -I./src -I./lib -I./lib/glm -I. -s WASM=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -O3 -o build/index.js --preload-file assets
	rm -f -- build/index.html
	cp index.html build/index.html
fi

if [ "$TARGET" = "native" ]; then
	cd "$ROOT_DIR"
	rm build/main
	g++ -g -Wno-volatile -std=c++20 src/platform.cpp lib/winx/winx.c lib/glad/glad.c $SOURCES -I./src -I./lib -I./lib/glm -I. -ldl -lGL -lX11 -lXcursor -lopenal -o build/main
fi

if [ "$RUN" = "true" ]; then
    ./build/main
fi
