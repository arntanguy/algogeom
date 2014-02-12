###############################################################################
#  CMakeFile for project I3S-CNRS AR Project
#  Author: Arnaud TANGUY
###############################################################################

SET(PROJECT_NAME CGEngine)
PROJECT(${PROJECT_NAME})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)
SET(CMAKE_BUILD_TYPE Debug)

# Set cpp flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CSS_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set (CMAKE_CXX_FLAGS -std=c++11)

# Path to the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeModules)

set(SRC src)
set(BIN_DIR .)
set(LIB_DIR lib)
set(LIB_TYPE SHARED)
#set(LIB_TYPE STATIC)
set(DOC_DIR doc)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

set(SYSTEM_LIBS
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARY}
)

set(EXTRA_LIBS
    "${LIB_DIR}/cgengine/lib/libCGEngine.so"
)

set(SYSTEM_INCLUDES
    ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR}
)

set( EXTRA_INCLUDES
)

### DEFINE NEEDED SOURCES
#file(GLOB_RECURSE
#     SOURCES
#     ${SRC}/*.cpp
#)
set(SOURCES
${SRC}/baseshader.cpp
${SRC}/basetexture.cpp
${SRC}/shader.cpp
${SRC}/vao.cpp
${SRC}/vbo.cpp
)

message(STATUS "Building sources ${SOURCES}")

#### BUILD INSTRUCTIONS
include_directories (${SRC} ${SYSTEM_INCLUDES})

### CREATE LIBRARY
add_library(${PROJECT_NAME} ${LIB_TYPE} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${SYSTEM_LIBS} ${EXTRA_LIBS})


# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
     configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile ${BIN_DIR}/Doxyfile @ONLY)
     add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${BIN_DIR}/Doxyfile
     WORKING_DIRECTORY ${DOC_DIR}
     COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

