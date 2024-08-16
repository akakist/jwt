cmake_minimum_required (VERSION 3.0)

add_library(jwt-node SHARED
jwtWebServer.cpp
jwtWebServerService.cpp
jwtWebServerService.h
)

target_link_libraries(jwt-node   megatron-core pthread)

install(TARGETS jwt-node DESTINATION ${PLUGIN_TARGET_DIR})

