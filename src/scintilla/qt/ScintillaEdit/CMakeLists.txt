cmake_minimum_required(VERSION 3.16)
project(ScintillaEdit VERSION 5.4.3 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Core5Compat Widgets)

qt_add_library(ScintillaEdit
    ../../src/AutoComplete.cxx
    ../../src/CallTip.cxx
    ../../src/CaseConvert.cxx
    ../../src/CaseFolder.cxx
    ../../src/CellBuffer.cxx
    ../../src/ChangeHistory.cxx
    ../../src/CharClassify.cxx
    ../../src/CharacterCategoryMap.cxx
    ../../src/CharacterType.cxx
    ../../src/ContractionState.cxx
    ../../src/DBCS.cxx
    ../../src/Decoration.cxx
    ../../src/Document.cxx
    ../../src/EditModel.cxx
    ../../src/EditView.cxx
    ../../src/Editor.cxx
    ../../src/Geometry.cxx
    ../../src/Indicator.cxx
    ../../src/KeyMap.cxx
    ../../src/LineMarker.cxx
    ../../src/MarginView.cxx
    ../../src/PerLine.cxx
    ../../src/PositionCache.cxx
    ../../src/RESearch.cxx
    ../../src/RunStyles.cxx
    ../../src/ScintillaBase.cxx
    ../../src/Selection.cxx
    ../../src/Style.cxx
    ../../src/UndoHistory.cxx
    ../../src/UniConversion.cxx
    ../../src/UniqueString.cxx
    ../../src/ViewStyle.cxx
    ../../src/XPM.cxx
    ../ScintillaEditBase/PlatQt.cpp
    ../ScintillaEditBase/ScintillaEditBase.cpp ../ScintillaEditBase/ScintillaEditBase.h
    ../ScintillaEditBase/ScintillaQt.cpp ../ScintillaEditBase/ScintillaQt.h
    ScintillaDocument.cpp ScintillaDocument.h
    ScintillaEdit.cpp ScintillaEdit.h

)
target_include_directories(ScintillaEdit PUBLIC
    ../../include
    ../../src
    ../ScintillaEditBase
)

target_compile_definitions(ScintillaEdit PUBLIC
    MAKING_LIBRARY=1
    SCINTILLA_QT=1
)

target_link_libraries(ScintillaEdit PUBLIC
    Qt::Core
    Qt::Gui
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(ScintillaEdit PUBLIC
        Qt::Widgets
    )
endif()

if((QT_VERSION_MAJOR EQUAL 6))
    target_link_libraries(ScintillaEdit PUBLIC
        Qt::Core5Compat
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_definitions(ScintillaEdit PUBLIC
        NDEBUG=1
    )
endif()

install(TARGETS ScintillaEdit
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
