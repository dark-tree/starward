
CYAN=`tput setaf 6`
BOLD=`tput bold`
NONE=`tput sgr0`

run_flag='false'
cmake_config='Debug'

ROOT_DIR="$(pwd)"
BUILD_DIR="${ROOT_DIR}/build"

if [ ! -d "build" ]; then
	mkdir build
fi

while getopts 'rx' flag; do
  case "${flag}" in
    r) run_flag='true' ;;
    x) cmake_config='Release' ;;
    *) print_usage
       exit 1 ;;
  esac
done

# check if emscripten SDK is present
if [ ! -d "$BUILD_DIR/emsdk" ]; then
	printf "${BOLD}Emscripten SDK not present, installing...\n${NONE}"
	sleep 1

  cd "$BUILD_DIR" || exit
	mkdir emsdk
	cd emsdk || exit 1

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

cd "${ROOT_DIR}" || exit
emcmake cmake . -B build -DCMAKE_BUILD_TYPE=$cmake_config || exit 1
cmake --build build/ -j 8 || exit 1

# Copy index file
rm -f -- build/index.html
cp index.html build/index.html

if [ "$run_flag" = true ]; then
  ./server.sh
fi