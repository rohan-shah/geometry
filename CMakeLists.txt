cmake_minimum_required(VERSION 3.1)
project(geometry)
if(CMAKE_COMPILER_IS_GNUCC)
	add_definitions("-Wall -Wno-unused-local-typedefs")
endif()

find_package(Boost 1.53.0 REQUIRED)
find_package(CGAL)

find_file(cml_HEADER_PATH cml/cml.h)
get_filename_component(cml_DIR "${cml_HEADER_PATH}" DIRECTORY)
set(cml_DIR "${cml_DIR}/..")
add_subdirectory(GeometryCommon)

export(TARGETS geometryCommon FILE geometryConfig.cmake)
