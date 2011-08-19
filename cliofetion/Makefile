default: cmake
	cd build;make

cmake:
	cd build;cmake ..

run:
	./cli.sh

all:
	gcc src/cliofetion.c -I../libofetion -I/usr/include/libxml2 -L../libofetion/build/ -lofetion -o build/cliofetion
