PROJECT(libopenfetion)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(TARNAME pidgin-ofetion)

SET(ICON_FILE "res/openfetion.png")
SET(RES_FILE "res/city.xml" "res/province.xml")

SET(SRC_LIST
	fx_blist.c
	fx_buddy.c
	fx_chat.c
	fx_contact.c
	fx_login.c
	fx_sip.c
	fx_user.c
	fx_util.c
	openfetion.c
)

IF(NOT PURPLE_MAJOR_VERSION)
	SET(PURPLE_MAJOR_VERSION "2")
ENDIF(NOT PURPLE_MAJOR_VERSION)

FIND_PACKAGE(PkgConfig REQUIRED)

PKG_CHECK_MODULES(LIBXML2 REQUIRED libxml-2.0)
PKG_CHECK_MODULES(OPENSSL REQUIRED openssl)
PKG_CHECK_MODULES(PURPLE REQUIRED  purple)

option(NLS "Native language support" ON)
if(NLS)
	find_package(Gettext)
endif(NLS)

ADD_DEFINITIONS(-Wall -Wextra -g)

ADD_LIBRARY(libopenfetion MODULE ${SRC_LIST})
set_target_properties(libopenfetion PROPERTIES PREFIX "")

INCLUDE_DIRECTORIES(
	${LIBXML2_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIRS}
	${PURPLE_INCLUDE_DIRS}
	)

TARGET_LINK_LIBRARIES(libopenfetion
   					${LIBXML_LIBRARIES}
					${OPENSSL_LIBRARIES}
					${PURPLE_LIBRARIES}
					)

EXEC_PROGRAM("whereis libpurple| sed -e 's/[^\\/]*//' -e 's/\\(.*\\)lib\\/libpurple\\.so.*/\\1/'"
			OUTPUT_VARIABLE PURPLE_PREFIX)

EXEC_PROGRAM("whereis pidgin | sed -e 's/[^\\/]*//' -e 's/\\(.*\\)bin\\/pidgin.*/\\1/'"
			OUTPUT_VARIABLE PIDGIN_PREFIX)

SET(PIDGIN_PIX_INSTALL_DIR "${PIDGIN_PREFIX}share/pixmaps/pidgin/protocols/16/")
SET(RES_INSTALL_DIR "${PURPLE_PREFIX}share/purple/openfetion/")
SET(LIB_INSTALL_DIR "${PURPLE_PREFIX}lib/purple-${PURPLE_MAJOR_VERSION}")
SET(LOCALE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/locale/")

ADD_DEFINITIONS(-DRES_DIR="${RES_INSTALL_DIR}")
if(NLS AND GETTEXT_FOUND)
	add_definitions(-DGETTEXT_PACKAGE="${TARNAME}")
	add_definitions(-DENABLE_NLS="1")
	message(STATUS "Native language support:  YES" )
else(NLS AND GETTEXT_FOUND)
	message(STATUS "Native language support:  NO" )
endif(NLS AND GETTEXT_FOUND)
add_definitions(-DLOCALE_DIR="${LOCALE_INSTALL_DIR}")

if(NLS AND GETTEXT_FOUND)
	file(GLOB POFILES "${CMAKE_CURRENT_SOURCE_DIR}/po/*.po")
	gettext_create_translations("${CMAKE_CURRENT_SOURCE_DIR}/po/pidgin-ofetion.pot" ALL ${POFILES})
endif(NLS AND GETTEXT_FOUND)

INSTALL(TARGETS libopenfetion DESTINATION ${LIB_INSTALL_DIR})
INSTALL(FILES ${ICON_FILE} DESTINATION ${PIDGIN_PIX_INSTALL_DIR})
INSTALL(FILES ${RES_FILE} DESTINATION ${RES_INSTALL_DIR})
