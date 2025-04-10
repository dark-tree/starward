if [ ! -d "build" ]; then
	./build.sh
fi

cd build
python3 -m http.server 8080
