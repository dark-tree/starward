cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(Starward)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm
		GIT_TAG        1.0.1
)

FetchContent_Declare(
		stb
		GIT_REPOSITORY https://github.com/nothings/stb
		GIT_TAG        f0569113c93ad095470c54bf34a17b36646bbbb5
)

FetchContent_MakeAvailable(glm stb)
set(libraries ${glm_SOURCE_DIR} ${stb_SOURCE_DIR} ${GLAD_INCLUDE_DIRS} ${OpenAL_INCLUDE_DIR})

file(GLOB_RECURSE GAME_SOURCES "src/**.cpp")
add_executable(main ${GAME_SOURCES})
target_include_directories(main PRIVATE lib src ${glm_SOURCE_DIR} ${stb_SOURCE_DIR} ${OpenAL_INCLUDE_DIR})

add_library(external "lib/implementation.cpp" "lib/font8x8.c")
target_include_directories(external PRIVATE ${stb_SOURCE_DIR})

if (DEFINED EMSCRIPTEN)

	message(STATUS "Building for EMSCRIPTEN")

	target_compile_options(main PRIVATE -O3 -Wno-c++17-extensions)
	target_link_options(main PRIVATE "-sEXPORTED_FUNCTIONS=[\"_free\", \"_main\"]" -sINITIAL_MEMORY=33554432 -sALLOW_MEMORY_GROWTH -sWASM=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 --preload-file assets)

	if (NOT ${CMAKE_BUILD_TYPE} MATCHES "Release")
		message("-- Building in Debug mode")
		target_link_options(main PRIVATE -sRUNTIME_DEBUG -sOPENAL_DEBUG)
	else()
		message("-- Building in Release mode")
	endif()

	target_link_libraries(main PRIVATE glm external openal)

else()

	find_package(OpenAL REQUIRED)
	message(STATUS "Building for NATIVE")

	FetchContent_Declare(
			winx
			GIT_REPOSITORY https://github.com/dark-tree/winx
			GIT_TAG        6bc5283274e4ceb714a8f551bc446aaa2991ec53
	)

	FetchContent_Declare(
			glad
			GIT_REPOSITORY https://github.com/Dav1dde/glad
			GIT_TAG        v0.1.36
	)

	set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
	set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")

	FetchContent_MakeAvailable(winx glad)

	target_link_libraries(main PRIVATE glm external winx glad OpenAL::OpenAL)
	target_include_directories(main PRIVATE ${winx_SOURCE_DIR} ${GLAD_INCLUDE_DIRS})

endif()

add_custom_target(copy-assets ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/assets"
		"${CMAKE_BINARY_DIR}/assets"
)

add_dependencies(main copy-assets)
