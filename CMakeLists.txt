cmake_minimum_required(VERSION 3.15)
project(	gap
					LANGUAGES C CXX 
					VERSION 0.0.4 )

configure_file(src/config.h.in src/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

cmake_policy(SET CMP0072 NEW)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()


include(FetchContent)

message(STATUS "Adding Ade PNG Library")
FetchContent_Declare(
		adepng
		GIT_REPOSITORY https://github.com/adrian-purser/adepng.git
		GIT_TAG origin/main
	)
FetchContent_MakeAvailable(adepng)

message(STATUS "Adding Ade FileSystem Library")
FetchContent_Declare(
		adefs
		GIT_REPOSITORY https://github.com/adrian-purser/adefs.git
	)
FetchContent_MakeAvailable(adefs)

message(STATUS "Adding Ade XML Library")
FetchContent_Declare(
	adexml
	GIT_REPOSITORY https://github.com/adrian-purser/adexml.git
	GIT_TAG origin/main
)
FetchContent_MakeAvailable(adexml)

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

include(cmake/StandardProjectSettings.cmake)
include(cmake/StaticAnalyzers.cmake)


add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME}
PRIVATE
	src/assets.cpp
	src/build.cpp
	src/configuration.cpp
	src/encode_definitions.cpp
	src/encode_gbin.cpp
	src/errors.cpp
	src/export.cpp
	src/image.cpp
	src/main.cpp
	src/parse_colour_map.cpp
	src/parse_gap.cpp
	src/source_tilemap.cpp
PUBLIC
	src/build.h
	src/configuration.h
	src/encode_definitions.h
	src/encode_gbin.h
	src/errors.h
	src/export.h
	src/image.h
	src/parse_colour_map.h
	src/parse_gap.h
	src/source_tilemap.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC adefs adepng adexml pthread dl)

add_subdirectory(src/tests)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
