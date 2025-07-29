cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(recompiler)


# Include libraries
# Rabbitizer
add_library(rabbitizer STATIC)
file(GLOB_RECURSE RABBIT_SOURCE_FILES ${CMAKE_SOURCE_DIR}/dependencies/rabbitizer/src/*.c)
target_sources(rabbitizer PRIVATE ${RABBIT_SOURCE_FILES})
target_include_directories(rabbitizer PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/rabbitizer/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/rabbitizer/tables"
)

# elf
file(GLOB_RECURSE ELF_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/elf/*.c)
add_library(elflib ${ELF_SOURCE})
target_include_directories(elflib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Common functions
file(GLOB_RECURSE COMMON_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.c")
add_library(common ${COMMON_SOURCE})
target_include_directories(common PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Define recompiler
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/ps2_rc/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/rabbitizer/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE rabbitizer yaml elflib common)

# PS2 Lib
# This is a collection of functions meant to operate with modern pcs
file(GLOB_RECURSE PS2_LIB_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/ps2lib/*.c")
add_library(ps2lib ${PS2_LIB_SOURCE})
target_include_directories(ps2lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Create recompiled app
file(GLOB_RECURSE RECOMPILED_SOURCE_FILES 
    "${CMAKE_BINARY_DIR}/generated/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/decomp/*.c"
)
list(LENGTH RECOMPILED_SOURCE_FILES recomp_source_num)
if (${recomp_source_num} GREATER 0) 
    add_executable(recompiled ${RECOMPILED_SOURCE_FILES})
    target_include_directories(recompiled PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated"
    )

    target_link_libraries(recompiled PRIVATE ps2lib common m)
    target_precompile_headers(recompiled PRIVATE
        "$<$<COMPILE_LANGUAGE:C>:ps2ctx/pch.h>"
        "$<$<COMPILE_LANGUAGE:C>:${CMAKE_BINARY_DIR}/generated/rc_functions.h>"
    )

    # target_link_libraries(${PROJECT_NAME} PRIVATE rabbitizer yaml)
endif()
