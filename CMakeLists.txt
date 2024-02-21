cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(GtkExample C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(GtkExample ${SRC_FILES})
target_include_directories(GtkExample PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

# Debug C/C++ flags
if(CMAKE_BUILD_TYPE STREQUAL Debug)
	if(NOT WIN32)
		add_compile_options(-fsanitize=address,undefined)
		add_link_options(-fsanitize=address,undefined)
	endif()
endif()

target_include_directories(GtkExample PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(GtkExample PRIVATE ${GTK_LIBRARY_DIRS})
target_link_libraries(GtkExample PRIVATE ${GTK_LIBRARIES})

target_compile_options(GtkExample PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

if(WIN32)
	add_custom_command(TARGET csprite POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:csprite> $<TARGET_RUNTIME_DLLS:csprite>
		COMMAND_EXPAND_LISTS
	)
endif()
