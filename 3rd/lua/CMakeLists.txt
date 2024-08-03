cmake_minimum_required(VERSION 3.10)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/linux)
else()
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/source-charset:utf-8>")
	#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/win)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SRC ./*.h  ./*.c)

add_library(lua STATIC ${SRC})
set_target_properties(lua PROPERTIES OUTPUT_NAME "lua")
