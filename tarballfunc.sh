#!/bin/bash
#***************************************************************************
#*   Copyright (C) 2011 by YunQiang Su                                     *
#*   wzssyqa@gmail.com                                                     *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU General Public License for more details.                          *
#*                                                                         *
#*   You should have received a copy of the GNU General Public License     *
#*   along with this program; if not, write to the                         *
#*   Free Software Foundation, Inc.,                                       *
#*   51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.            *
#*                                                                         *
#***************************************************************************



VERSION=2.1.0
LIBOFETION_VERSION=$VERSION
CLIOFETION_VERSION=$VERSION
OPENFETION_VERSION=$VERSION
API_VERSION=1.1.0
ABI_VERSION=1.0.0

create_tmp_dir(){
	if [ -d tarball ];then
		rm -rf tarball/*
	else 
		mkdir tarball
	fi
}

sha256sum_gpg(){
	cd tarball
	sha256sum *.tar.gz >sha256sum.txt
	gpg --clearsign sha256sum.txt
	cd ..
}

process_version(){
	#set version of cliofetion CMakeLists.txt
	tmp=`sed -n '/^set(PACKAGE_VERSION/p' cliofetion/CMakeLists.txt`
	sed -i -e "s,$tmp,set(PACKAGE_VERSION \"$CLIOFETION_VERSION\"),g" cliofetion/CMakeLists.txt
	
	#set version of openfetion CMakeLists.txt
	tmp=`sed -n '/^set(PACKAGE_VERSION/p' openfetion/CMakeLists.txt`
	sed -i -e "s,$tmp,set(PACKAGE_VERSION \"$OPENFETION_VERSION\"),g" openfetion/CMakeLists.txt
	
	#openfetion version define in openfetion/src/fx_about.h
	tmp=`sed -n '/^#define OPENFETION_VERSION/p' openfetion/src/fx_about.h`
	sed -i -e "s,$tmp,#define OPENFETION_VERSION \"$OPENFETION_VERSION\",g" openfetion/src/fx_about.h

	#set version of libofetion CMakeLists.txt
	tmp=`sed -n '/^set(LIBOFETION_API_VERSION/p' libofetion/CMakeLists.txt`
	sed -i -e "s,$tmp,set(LIBOFETION_API_VERSION \"$API_VERSION\"),g" libofetion/CMakeLists.txt
	
	tmp_ABI=`echo $ABI_VERSION | awk -F '.' '{print   $1}'`
	tmp=`sed -n '/^set(LIBOFETION_ABI_MAJOR_VERSION/p' libofetion/CMakeLists.txt`
	sed -i -e "s,$tmp,set(LIBOFETION_ABI_MAJOR_VERSION \"$tmp_ABI\"),g" libofetion/CMakeLists.txt
	
	tmp=`sed -n '/^set(LIBOFETION_ABI_VERSION/p' libofetion/CMakeLists.txt`
	sed -i -e "s,$tmp,set(LIBOFETION_ABI_VERSION \"$ABI_VERSION\"),g" libofetion/CMakeLists.txt
	
	#libofetion version define in libofetion/fetion_types.h
	tmp=`sed -n '/^#define FETION_VERSION/p' libofetion/fetion_types.h`
	sed -i -e "s,$tmp,#define FETION_VERSION \"$LIBOETION_VERSION\",g" libofetion/fetion_types.h
	
	#ABI_VERSION in libofetion/libofetion.map
	tmp=`sed -n '/^VERS_/p' libofetion/libofetion.map`
	sed -i -e "s,$tmp,VERS_$OPENFETION_VERSION {,g" libofetion/fetion_types.h	
}

compress_tar(){
	cp -r $1 tarball/$1-$2
	cd tarball
	tar zcvf $1-$2.tar.gz $1-$2 >/dev/null
	rm -rf $1-$2
	cd ..
}

test_tarball(){
	tar -zxvf ../$1-$2.tar.gz >/dev/null
	cd $1-$2
	mkdir build
	cd build
	cmake .. -DCMAKE_INSTALL_PREFIX=ff
	make
	make install
	cd ../..
}

gen_tarball(){
	########################################################################
	create_tmp_dir
	process_version

	########################################################################
	# libofetion
	TARNAME=libofetion

	compress_tar $TARNAME $LIBOFETION_VERSION

	################################################################################
	# openfetion, requires libofetion from system, GTK+
	TARNAME=openfetion

	compress_tar $TARNAME $OPENFETION_VERSION

	################################################################################
	# cliofetion requires libofetion from system
	TARNAME=cliofetion

	compress_tar $TARNAME $CLIOFETION_VERSION
}


################################################################################
copy_standalone_files(){
	drc=$1-standalone-$2
	cp -r $1 tarball/$drc
	rm tarball/$drc/INSTALL.standalone
	cp $1/INSTALL.standalone tarball/$drc/INSTALL
	mkdir -p tarball/$drc/lib
	cp libofetion/*.h tarball/$drc/lib
	cp libofetion/*.c tarball/$drc/lib
	mkdir -p tarball/$drc/resource
	cp libofetion/resource/*.xml tarball/$drc/resource
}

patch_standalone_check_ofetion(){
	drc=$1-standalone-$2
	tmp=`sed -n '/^pkg_check_modules(OFETION REQUIRED ofetion)/p' tarball/$drc/CMakeLists.txt`
	/bin/echo -n "s,$tmp,pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)\n" >tmpfile
	/bin/echo -n "pkg_check_modules(OPENSSL REQUIRED openssl)\n" >>tmpfile
	/bin/echo -n "pkg_check_modules(SQLITE3 REQUIRED sqlite3),g" >>tmpfile
	sed -i -f tmpfile tarball/$drc/CMakeLists.txt
	rm tmpfile
	
	/bin/echo -n 's,    ${OFETION_INCLUDE_DIRS},    ${LIBXML2_INCLUDE_DIRS}\n' >tmpfile
	/bin/echo -n '    ${OPENSSL_INCLUDE_DIRS}\n' >>tmpfile
	/bin/echo -n '    ${SQLITE3_INCLUDE_DIRS}\n' >>tmpfile
	/bin/echo -n '    ${SQLITE3_INCLUDE_DIRS}\n' >>tmpfile
	/bin/echo -n '    ${CMAKE_CURRENT_SOURCE_DIR}\n' >>tmpfile
	/bin/echo -n '    ${CMAKE_CURRENT_SOURCE_DIR}/lib\n' >>tmpfile
	/bin/echo -n '    ${CMAKE_CURRENT_BINARY_DIR},g' >>tmpfile
	sed -i -f tmpfile tarball/$drc/CMakeLists.txt
	rm tmpfile
	
	/bin/echo -n "s,set($1_SRC,set($1_SRC\n" >tmpfile
	
	for i in `cd tarball/$drc;ls lib/*.c;cd ../..`
	do
		/bin/echo -n "    $i\n" >>tmpfile
	done
	
	echo ",g" >>tmpfile
	sed -i -f tmpfile tarball/$drc/CMakeLists.txt
	rm tmpfile
	
	sed -i -e 's,${OFETION_LIBRARIES},${LIBXML2_LIBRARIES} ${OPENSSL_LIBRARIES} ${SQLITE3_LIBRARIES},g' tarball/$drc/CMakeLists.txt
	
	echo 'install(FILES resource/city.xml DESTINATION ${DATA_INSTALL_DIR}/openfetion/resource)' >>tarball/$drc/CMakeLists.txt
	echo 'install(FILES resource/province.xml DESTINATION ${DATA_INSTALL_DIR}/openfetion/resource)' >>tarball/$drc/CMakeLists.txt
}

compress_standalone_tarball(){
	drc=$1-standalone-$2
	cd tarball
	tar zcvf $drc.tar.gz $drc >/dev/null
	sha256sum $drc.tar.gz >>tar256sum.txt
	rm -rf $drc
	cd ..
}

gen_standalone_tarball(){
	copy_standalone_files cliofetion $CLIOFETION_VERSION
	copy_standalone_files openfetion $OPENFETION_VERSION
	
	patch_standalone_check_ofetion cliofetion $CLIOFETION_VERSION
	patch_standalone_check_ofetion openfetion $OPENFETION_VERSION
	
	compress_standalone_tarball cliofetion $CLIOFETION_VERSION
	compress_standalone_tarball openfetion $OPENFETION_VERSION
}



do_test(){
	

	cd tarball
	mkdir -p test
	cd test
	rm -rf *

	test_tarball libofetion $LIBOFETION_VERSION
	test_tarball openfetion $OPENFETION_VERSION
	test_tarball cliofetion $CLIOFETION_VERSION
	test_tarball cliofetion-standalone $CLIOFETION_VERSION
	test_tarball openfetion-standalone $OPENFETION_VERSION
}
