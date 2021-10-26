cmake_minimum_required(VERSION 3.15)
project(gap)

cmake_policy(SET CMP0072 NEW)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

include(cmake/StandardProjectSettings.cmake)
include(cmake/StaticAnalyzers.cmake)

set(SOURCES
	src/app.cpp
	src/assets.cpp
	src/configuration.cpp
	src/encode_gbin.cpp
	src/export.cpp
	src/image.cpp
	src/main.cpp
	src/parse_gap.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC adefs adepng pthread fmt dl)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/adefs)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/adepng)

