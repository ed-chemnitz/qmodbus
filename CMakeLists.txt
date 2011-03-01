CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

PROJECT(QModBus)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0005 NEW)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "2")
SET(VERSION_PATCH "1")
#SET(VERSION_SUFFIX "")

SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)

# check for Qt4
SET(QT_MIN_VERSION "4.4.0")
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})


#CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/qmodbusconfig.h.in ${CMAKE_BINARY_DIR}/qmodbusconfig.h)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/qmodbus.rc.in ${CMAKE_BINARY_DIR}/qmodbus.rc)

# set compiler flags
SET(CMAKE_C_FLAGS "-O2 -g -Wall ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "-O2 -g -fno-exceptions -Wall ${CMAKE_CXX_FLAGS}")

SET(qmodbus_SOURCES src/main.cpp 
    src/mainwindow.cpp 
    3rdparty/qextserialport/qextserialport.cpp 
	3rdparty/libmodbus/src/modbus.c
	3rdparty/libmodbus/src/modbus-data.c
	3rdparty/libmodbus/src/modbus-rtu.c
	3rdparty/libmodbus/src/modbus-tcp.c)

SET(qmodbus_INCLUDES src/mainwindow.h 
    3rdparty/qextserialport/qextserialport.h 
    3rdparty/qextserialport/qextserialenumerator.h 
    3rdparty/libmodbus/src/modbus.h)

IF(WIN32)
	SET(qmodbus_SOURCES ${qmodbus_SOURCES} 3rdparty/qextserialport/win_qextserialport.cpp 3rdparty/qextserialport/qextserialenumerator_win.cpp)
	ADD_DEFINITIONS(-D_TTY_WIN_)
	LINK_LIBRARIES(-lsetupapi -lws2_32)
	INSTALL(FILES ${QT_BINARY_DIR}/QtCore4.dll ${QT_BINARY_DIR}/QtGui4.dll ${QT_BINARY_DIR}/zlib1.dll DESTINATION .)
	SET(WINRC "${CMAKE_BINARY_DIR}/qmodbusrc.obj")
	ADD_CUSTOM_COMMAND(OUTPUT ${WINRC}
			COMMAND ${WINDRES}
				-I${CMAKE_SOURCE_DIR}
				-o${CMAKE_BINARY_DIR}/qmodbusrc.obj
				-i${CMAKE_BINARY_DIR}/qmodbus.rc
			DEPENDS ${CMAKE_BINARY_DIR}/qmodbus.rc)
ELSE(WIN32)
	SET(qmodbus_SOURCES ${qmodbus_SOURCES} 3rdparty/qextserialport/posix_qextserialport.cpp 3rdparty/qextserialport/qextserialenumerator_unix.cpp)
	ADD_DEFINITIONS(-D_TTY_POSIX_)
ENDIF(WIN32)

SET(qmodbus_UI forms/mainwindow.ui forms/about.ui)

QT4_WRAP_CPP(qmodbus_MOC_out ${qmodbus_INCLUDES})
QT4_WRAP_UI(qmodbus_UIC_out ${qmodbus_UI})
QT4_ADD_RESOURCES(qmodbus_RCC_out data/qmodbus.qrc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libmodbus ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libmodbus/src ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qextserialport ${CMAKE_BINARY_DIR})
LINK_LIBRARIES(${QT_LIBRARIES})
ADD_EXECUTABLE(qmodbus ${qmodbus_SOURCES} ${qmodbus_UIC_out} ${qmodbus_MOC_out} ${qmodbus_RCC_out} ${WINRC})

IF(WIN32)
	SET_TARGET_PROPERTIES(qmodbus PROPERTIES LINK_FLAGS "${LINK_FLAGS} -mwindows")
        ADD_CUSTOM_COMMAND(TARGET qmodbus POST_BUILD COMMAND ${STRIP} ${CMAKE_BINARY_DIR}/qmodbus.exe)
        INSTALL(TARGETS qmodbus RUNTIME DESTINATION .)
ELSE(WIN32)
        INSTALL(TARGETS qmodbus RUNTIME DESTINATION bin)
ENDIF(WIN32)

#
# add dist target
#
SET(TMP "qmodbus-${VERSION}")
ADD_CUSTOM_TARGET(dist
                        COMMAND make clean
                        COMMAND rm -rf ${TMP}
                        COMMAND mkdir -p ${TMP}
                        COMMAND cp AUTHORS build_mingw32 CMakeLists.txt qmodbus.pro COPYING INSTALL qmodbus.rc.in README TODO ${TMP}
                        COMMAND cp -r 3rdparty cmake data forms src ${TMP}
                        COMMAND rm -rf `find ${TMP} -name cmake_install.cmake` `find ${TMP} -name Makefile` `find ${TMP} -type d -name CMakeFiles` ${TMP}/CMakeCache.txt
                        COMMAND tar cjf qmodbus-${VERSION}.tar.bz2 ${TMP}
                        COMMAND rm -rf ${TMP})


INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QModBus")
SET(CPACK_PACKAGE_VENDOR "Electronic Design Chemnitz (EDC) - http://www.ed-chemnitz.de")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "QModBus ${VERSION}")
IF(WIN32)
SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/data\\\\qmodbus.ico")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "qmodbus.exe")
SET(CPACK_NSIS_DISPLAY_NAME "QModBus")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\qmodbus.sourceforge.net")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\qmodbus.sourceforge.net")
SET(CPACK_NSIS_CONTACT "tobias.doerffel@gmail.com")
SET(CPACK_PACKAGE_EXECUTABLES "qmodbus.exe;QModBus")
SET(CPACK_NSIS_MENU_LINKS "qmodbus.exe;QModBus")
ELSE(WIN32)
SET(CPACK_STRIP_FILES "bin/qmodbus")
SET(CPACK_PACKAGE_EXECUTABLES "qmodbus" "QModBus binary")
ENDIF(WIN32)

SET(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/data/logo.png")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "QModBus")
SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_NAME "QModBus")
SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}")
SET(MACOSX_BUNDLE_COPYRIGHT "Tobias Doerffel, 2009")

SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "qmodbus-${VERSION}")
INCLUDE(CPack)

