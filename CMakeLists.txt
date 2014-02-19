set(PROJECT_NAME miniprojet)
project (${PROJECT_NAME})

cmake_minimum_required(VERSION 2.8)

set(SRC src)
set(LIB_DIR .)
set(INCLUDES src "lib/cgengine/src")

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

if( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
endif()

set( QT_USE_QTXML    TRUE )
set( QT_USE_QTMAIN   TRUE )
set( QT_USE_QTSCRIPT  TRUE )
set( QT_USE_QTOPENGL  TRUE )

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

find_package(CGAL COMPONENTS)
find_package(Qt4)
find_package(GLUT)
find_package(OpenGL)
find_package(GLEW)
find_package(OpenCV REQUIRED COMPONENTS core ml features2d highgui nonfree)

#message( STATUS "********CXXflags: " ${CMAKE_CXX_FLAGS} )
include(${CGAL_USE_FILE})#change CXX flags
#message( STATUS "********CXXflags: " ${CMAKE_CXX_FLAGS} )
include(${QT_USE_FILE})#change compile flag
#message( STATUS "********CXXflags: " ${CMAKE_CXX_FLAGS} )

#add_definitions(-march=native -ffast-math)
#add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
message( STATUS "CXXflags: " ${CMAKE_CXX_FLAGS} )

set(SYSTEM_INCLUDES 
	${OPENCV_INCLUDE_DIRS}
)

set( SRCS 
${SRC}/glviewer.cpp 
${SRC}/main.cpp 
${SRC}/window.cpp
${SRC}/scene.cpp
${SRC}/qttexture.cpp
${SRC}/Utils.cpp
) 

set( SRC_FUSION ${SRC}/main_fusion.cpp)
set(SRC_TEST
	${SRC}/test_scene.cpp
	${SRC}/scene.cpp
)

set(SRC_MOC ${SRC}/window.h ${SRC}/glviewer.h)

if(WIN32)
	add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4503 /nologo" )
endif(WIN32)



if( CGAL_FOUND AND QT4_FOUND AND OPENGL_FOUND )

    # UI files (Qt Designer files)
    qt4_wrap_ui( QT_UI_FILES ${SRC}/convex.ui)

    # qrc files (resources files, that contain icons, at least)
    #qt4_add_resources( T_RESOURCE_FILES ${PROJECT_NAME}.qrc )

    # use the Qt MOC preprocessor on classes that derives from QObject
    qt4_wrap_cpp(QT_MOCS ${SRC_MOC} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED OPTIONS)

    # use the Qt MOC preprocessor on classes that derives from QObject
    #qt4_generate_moc( window.h moc_window.cxx )
    #qt4_generate_moc( glviewer.h moc_glviewer.cxx )

	include_directories(${INCLUDES}, ${SYSTEM_INCLUDES}, ${SRC})
    link_directories("${CMAKE_CURRENT_LIST_DIR}/lib/cgengine/lib")

    # The executable itself.
    add_executable( ${PROJECT_NAME} ${SRCS} ${QT_MOCS} ${QT_UI_FILES} ${QT_RESOURCE_FILES} )
	add_executable(fusion_file ${SRC_FUSION})
	add_executable(test_scene ${SRC_TEST})
    # Link with Qt libraries
    target_link_libraries( ${PROJECT_NAME} ${QT_LIBRARIES} )

    # Link with Glut and OpenGL
    target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARY} ${GLUT_LIBRARY}  ${GLEW_LIBRARY} )

    # Link with CGAL
    target_link_libraries( ${PROJECT_NAME} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
    target_link_libraries( test_scene ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

	#Link with opencv
    target_link_libraries( test_scene ${OPENCV_LIBRARIES})
    # XXX
    target_link_libraries( ${PROJECT_NAME} CGEngine )


else()
  message(STATUS "NOTICE: This program requires CGAL and Qt4, and will not be compiled.")

endif()
