# top-level
cmake_minimum_required(VERSION 3.21)
project(ijson)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(${TARGET_NAME} PROPERTIES RELEASE_POSTFIX "")

if (("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") OR ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo") )
    message("build type: " ${CMAKE_BUILD_TYPE})
    add_definitions(-D_DEBUG)
    set(QT_STATIC_PATH "d:/ops/qt-6.7.0/static")
    ## SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    ## SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Debug用DLL运行库，Release用静态链接运行库
    ## set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")
else ()
    message("build type: " "Release")
    set(QT_STATIC_PATH "d:/ops/qt-6.7.0/static")
    # Debug、Release都用静态链接运行库，推荐用于不想让用户装各种VC++运行库的软件
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

list(APPEND CMAKE_PREFIX_PATH ${QT_STATIC_PATH})

include_directories(${QT_STATIC_PATH}/include
        ${QT_STATIC_PATH}/include/QtCore
        ${QT_STATIC_PATH}/include/QtWidgets
        ${QT_STATIC_PATH}/include/QtGui
        ${QT_STATIC_PATH}/include/QtSvg
        ${QT_STATIC_PATH}/include/QtXml)

add_subdirectory(src/app)
add_subdirectory(src/scintilla/qt/ScintillaEdit)
