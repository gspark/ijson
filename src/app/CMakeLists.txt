# app
cmake_minimum_required(VERSION 3.21)
project(app LANGUAGES CXX C VERSION 0.0.6.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(MAKING_LIBRARY=1)
add_compile_definitions(SCINTILLA_QT=1)

#SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)

# 设置自动生成moc文件
set(CMAKE_AUTOMOC ON)
# 设置自动处理.qrc文件
set(CMAKE_AUTORCC ON)
# 设置自动生成ui.h文件
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Svg)
find_package(Qt6 COMPONENTS Xml)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Concurrent)

get_filename_component(TOP_DIR ${PROJECT_SOURCE_DIR}/../.. ABSOLUTE)

SET(RCC_FILES ${TOP_DIR}/resources/app.qrc
    ${TOP_DIR}/resources/icons/app.rc
    ${TOP_DIR}/resources/qss_icons/dark/darkstyle.qrc

)

if(MSVC)
    set(MY_VERSIONINFO_RC "${PROJECT_BINARY_DIR}/versionInfo.rc")
    configure_file("${TOP_DIR}/resources/version/versionInfo.rc.in" "${MY_VERSIONINFO_RC}")
endif()

include_directories(
     d:/ops/lexilla/include
    ../scintilla/qt/ScintillaEdit
    ./other
)

file(GLOB_RECURSE all_src_file
        ./*.cpp ./*.hpp ./*.c ./*.h ./*.ui
        ./other/nlohmann/*.hpp
        ./other/magic_enum/*.hpp
        )

add_executable(${PROJECT_NAME}
        ${all_src_file}
        ${RCC_FILES}
        ${MY_VERSIONINFO_RC}
        )

add_library(lexilla STATIC IMPORTED)

if (("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") OR ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo") )
    set_target_properties(lexilla PROPERTIES IMPORTED_LOCATION d:/ops/lexilla/lib/debug/liblexilla.lib)
else ()
    set_target_properties(lexilla PROPERTIES IMPORTED_LOCATION d:/ops/lexilla/lib/release/liblexilla.lib)
endif ()


set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE FALSE
        )

#link_directories(${CMAKE_CACHEFILE_DIR}/lib)

target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Xml
        Qt::Svg
        ScintillaEdit
        lexilla
        )
