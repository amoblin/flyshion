SERVER=ubuntu-edu.unix-center.net
FEDORA=fedora-edu.unix-center.net

default:cmake
	cd build;make

run:
	bistoo.sh

cmake:
	cd build;cmake ..

bistu_robot:
	gcc robot.c -I../libofetion -I/usr/include/libxml2 -L../libofetion/build/ -lofetion -o build/bin/$@

scp:
	-@rm -rf build ../libofetion/build
	scp -r . amoblin@$(SERVER):~/robot
	scp -r ../libofetion amoblin@$(SERVER):~

ssh:
	ssh amoblin@$(SERVER)
