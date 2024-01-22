
if [ ! -d "build" ]; then
	mkdir build
fi

cd build

if [ ! -d "emsdk" ]; then
	echo "Emscripten SDK not present, installing..."
	sleep 1

	mkdir emsdk
	cd emsdk

	git clone https://github.com/emscripten-core/emsdk.git .
	./emsdk install latest
	./emsdk activate latest

	cd ..
fi

source ./emsdk/emsdk_env.sh
cd ..

emcc src/main.cpp -I./src -I. -std=c++11 -s WASM=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -O3 -o build/index.js --preload-file assets

rm -f -- build/index.html
cp index.html build/index.html
