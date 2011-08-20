SERVER=ubuntu-edu.unix-center.net

default:cmake
	cd build;make

run:
	./robot.sh

cmake:
	cd build;cmake ..
all:
	gcc src/cliofetion.c -I../libofetion -I/usr/include/libxml2 -L../libofetion/build/ -lofetion -o build/bin/cliofetion

scp:
	-@rm -rf build ../libofetion/build
	scp -r . amoblin@$(SERVER):~/robot
	scp -r ../libofetion amoblin@$(SERVER):~

ssh:
	ssh amoblin@$(SERVER)
