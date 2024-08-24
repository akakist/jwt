cmake_minimum_required (VERSION 3.0)

add_library(jwt-Client SHARED
jwtClient.cpp
jwtClientService.cpp
jwtClientService.h
)

target_link_libraries(jwt-Client   megatron-core pthread)

install(TARGETS jwt-Client DESTINATION ${PLUGIN_TARGET_DIR})

