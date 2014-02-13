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

find_package(CGAL COMPONENTS)
find_package(Qt4)
find_package(GLUT)
find_package(OpenGL)
find_package(GLEW)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set( SRCS 
${SRC}/glviewer.cpp 
${SRC}/main.cpp 
${SRC}/window.cpp
${SRC}/scene.cpp
${SRC}/qttexture.cpp
) 

set(SRC_MOC ${SRC}/window.h ${SRC}/glviewer.h)

if(WIN32)
	add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4503 /nologo" )
endif(WIN32)



if( CGAL_FOUND AND QT4_FOUND AND OPENGL_FOUND )
    include(${CGAL_USE_FILE})
    include(${QT_USE_FILE})

    # UI files (Qt Designer files)
    qt4_wrap_ui( DT_UI_FILES ${SRC}/convex.ui)

    # qrc files (resources files, that contain icons, at least)
    #qt4_add_resources( DT_RESOURCE_FILES ${PROJECT_NAME}.qrc )

    # use the Qt MOC preprocessor on classes that derives from QObject
    qt4_wrap_cpp(MOCS ${SRC_MOC} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED OPTIONS)

    # use the Qt MOC preprocessor on classes that derives from QObject
    #qt4_generate_moc( window.h moc_window.cxx )
    #qt4_generate_moc( glviewer.h moc_glviewer.cxx )

    include_directories(${INCLUDES})
    link_directories("/home/arnaud/polytech/AlgoGeom/mini-projet/lib/cgengine/lib")

    # The executable itself.
    add_executable( ${PROJECT_NAME} ${SRCS} ${MOCS} ${DT_UI_FILES} ${DT_RESOURCE_FILES} )

    # Link with Qt libraries
    target_link_libraries( ${PROJECT_NAME} ${QT_LIBRARIES} )

    # Link with Glut and OpenGL
    target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARY} ${GLUT_LIBRARY}  ${GLEW_LIBRARY} )

    # Link with CGAL
    target_link_libraries( ${PROJECT_NAME} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

    # XXX
    target_link_libraries( ${PROJECT_NAME} CGEngine )


else()
  message(STATUS "NOTICE: This program requires CGAL and Qt4, and will not be compiled.")

endif()
