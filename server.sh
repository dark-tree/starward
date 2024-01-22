if [ ! -d "build" ]; then
	mkdir build
fi

cd build
python3 -m http.server 8080
