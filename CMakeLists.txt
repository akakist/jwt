

cmake_minimum_required (VERSION 3.16)

project (jwt)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()


set(M1 true)


SET(ENABLE_MYSQL true)
#SET(FreeBSD true)
SET(DFS true)
if(LINUX)
#    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.15.2/gcc_64)
    set(CMAKE_PREFIX_PATH /usr/lib/qt5/bin)
    set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --export-dynamic -Xlinker -zdefs")
else()
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.15.2/clang_64)
endif()

    include(CTest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)




OPTION(MT_CORE_SERVICES_SINGLE_PLUGING "Build apps/core-plugins - all core plugins in one file" ON)

set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl@3/3.1.1_1)
include_directories(/usr/local/Cellar/openssl@3/3.1.1_1/include)

set (OPENSSL_INCLUDE_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1t/include) 
set (OPENSSL_LIB_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1t/lib)
link_directories(${OPENSSL_LIB_DIR} /opt/homebrew/Cellar/mysql-client/8.3.0/lib)
include_directories(${OPENSSL_INCLUDE_DIR} local /usr/local lib/jsoncpp-0.5.0/include . corelib lib/SQLiteCpp lib main
    /opt/homebrew/Cellar/mysql-client/8.3.0/include
    )

#/usr/local/Cellar/openssl@3/3.1.1_1/include

#find_package(OpenSSL REQUIRED)

include_directories(megatron/corelib 
			megatron/main 
			megatron/local  
			megatron/lib/jsoncpp-0.5.0/include 
			megatron/lib/jpeg-9d
#			${OPENSSL_INCLUDE_DIR}
			/usr/local/Cellar/openssl@3/3.0.7/include
			/opt/homebrew/Cellar/mysql-client/8.0.32/include
			/opt/homebrew/Cellar/libharu/2.4.3/include
			megatron  .  svs megatron/lib megatron/lib/sqlite3
			/opt/homebrew/Cellar/openssl@3/3.0.8/include
			/opt/homebrew/Cellar/boost/1.81.0_1/include
			/usr/local/Cellar/openssl@3/3.1.3/include
			/usr/local/Cellar/openssl@3/3.2.0_1/include
                        /opt/homebrew/Cellar/mysql-client/8.2.0/include
			)

if(M1)
    link_directories(
	/opt/homebrew/Cellar/mysql-client/8.0.32/lib
	/opt/homebrew/Cellar/openssl@3/3.0.8/lib
    /opt/homebrew/Cellar/libharu/2.4.3/lib
    /opt/homebrew/Cellar/mysql-client/8.2.0/lib
    )
endif()
include_directories(system /usr/local/include)
#target_include_directories(foo PRIVATE /usr/local/include)
#INTERFACE_SYSTEM_INCLUDE_DIRECTORIES(/usr/local/include)

#add_definitions(-std=gnu++0x)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O3")
# -g -ggdb3 -pg
link_directories( /usr/local/lib /opt/homebrew/Cellar/openssl@3/3.0.8/lib /usr/local/Cellar/openssl@3/3.1.3/lib)
#link_directories(~/bin/lib /usr/local/lib)
IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF()
#SET(ENABLE_QT_GUI on)
SET(DISABLE_UPNP on)
#SET(ENABLE_GTEST on)

IF(ENABLE_QT_GUI)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)







if (APPLE) 
      set(CMAKE_MACOSX_RPATH ON) 
      SET(CMAKE_SKIP_BUILD_RPATH TRUE) 
      SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
      SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) 
endif () 

ENDIF()


SET(CAPS_IP "x.testquant.com")
#SET(CAPS_IP "127.0.0.1")
SET(PORT 10100)
#SET(CLIENTPORT 10100)

IF(ENABLE_QT_GUI)
    add_definitions(  -DQT5 -D__DDRAW)
    message(qt5)
ENDIF()

IF(LINUX)
SET(DL "dl")
ENDIF()

SET(PROJECT_BASE_DIR ~/bin-${PORT})
SET(PLUGIN_TARGET_DIR ${PROJECT_BASE_DIR}/plugin)
SET(BIN_TARGET_DIR ${PROJECT_BASE_DIR}/bin)
SET(LIB_TARGET_DIR ${PROJECT_BASE_DIR}/lib)
SET(LOG_TARGET_DIR ${PROJECT_BASE_DIR}/log)
SET(DATA_TARGET_DIR ${PROJECT_BASE_DIR}/data)
SET(CONFIG_TARGET_DIR ${PROJECT_BASE_DIR}/conf)
SET(CACHE_TARGET_DIR ${PROJECT_BASE_DIR}/cache)


CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/megatron/megatron_config.h.in"
                "${PROJECT_SOURCE_DIR}/megatron/megatron_config.h"
                NEWLINE_STYLE UNIX )



#set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE )
add_definitions(  -D__WITH_ZLIB  -DUSE_JSONASSERT -DQT_NO_VERSION_TAGGING )
#-DDEBUG  -DMUTEX_INSPECTOR_DEBUG
#-DMUTEX_INSPECTOR_DEBUG -DDEBUG
IF(DISABLE_UPNP)
#    add_definitions(  -DWITHOUT_UPNP  -DMUTEX_INSPECTOR_DEBUG )
ENDIF()
#-DMUTEX_INSPECTOR_DEBUG  -DWITH_SLOG -DMUTEX_INSPECTOR_DEBUG
#add_definitions(-O3)     -DWITH_ZXING -DWITH_ZBAR  -DWITHOUT_UPNP  
#-DDEBUG  -DMUTEX_INSPECTOR_DEBUG -DDEBUG

#find_library(GTest REQUIRED)

ADD_SUBDIRECTORY(megatron)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/jwt  ${CMAKE_CURRENT_BINARY_DIR}/jwt)

