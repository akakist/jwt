cmake_minimum_required (VERSION 3.0)

add_library(jwt-boss SHARED

jwtBoss.cpp
jwtBossService.cpp
jwtBossService.h

)

target_link_libraries(jwt-boss   megatron-core pthread)

install(TARGETS jwt-boss DESTINATION ${PLUGIN_TARGET_DIR})

