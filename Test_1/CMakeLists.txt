cmake_minimum_required(VERSION 3.5)

project(Test_1 VERSION 0.1 LANGUAGES CXX)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(
    hex_model
    hex_chess
    hex_chess/mode
    hex_ui
)

include_directories(
    ai_mcts
    ai_mcts/.a
    ai_mcts/.e
    ai_mcts/.g
)

set(UI
    hex_ui/MainWindow.h
    hex_ui/MainWindow.cpp
    hex_ui/StartWidget.h
    hex_ui/StartWidget.cpp
    hex_ui/StartWidget.ui
    hex_ui/ToolsWidget.h
    hex_ui/ToolsWidget.cpp
    hex_ui/ToolsWidget.ui
)

set(MODEL_SOURCES
    hex_model/HexAI.hpp
    hex_model/HexAI.cpp

    hex_model/HexIterator.hpp
    hex_model/HexArrayT.hpp
    hex_model/HexCell.hpp
    hex_model/HexPoint.hpp
    hex_model/HexCellArray.hpp
    hex_model/HexBoard.hpp
    hex_model/HexBoard.cpp
)

set(CHESS_SOURCES
    hex_chess/mode/RouteNode.hpp
    hex_chess/mode/RouteGraph.h
    hex_chess/mode/RouteGraph.cpp

    hex_chess/mode/GameMode.h
    hex_chess/mode/GameMode.cpp
    hex_chess/mode/GamePvP.h
    hex_chess/mode/GamePvP.cpp
    hex_chess/mode/GamePvE.h
    hex_chess/mode/GamePvE.cpp
    hex_chess/mode/GameEvE.h
    hex_chess/mode/GameEvE.cpp
    hex_chess/mode/GameDebug.h
    hex_chess/mode/GameDebug.cpp

    hex_chess/ChessBoard.h
    hex_chess/ChessBoard.cpp
)

set(AI_MCTS_SOURCES
    ai_mcts/MctsNode.hpp

    ai_mcts/.a/AI_Mcts_A.h
    ai_mcts/.a/AI_Mcts_A.cpp


    ai_mcts/.e/MctsWork_E.h
    ai_mcts/.e/MctsWork_E.cpp
    ai_mcts/.e/AI_Mcts_E.h
    ai_mcts/.e/AI_Mcts_E.cpp

    ai_mcts/.g/MctsWork_G.h
    ai_mcts/.g/MctsWork_G.cpp
    ai_mcts/.g/AI_Mcts_G.h
    ai_mcts/.g/AI_Mcts_G.cpp
)

set(PROJECT_SOURCES
    main.cpp
    ${MODEL_SOURCES}
    ${CHESS_SOURCES}
    ${AI_MCTS_SOURCES}
    ${UI}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Test_1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Test_1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Test_1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Test_1
            ${PROJECT_SOURCES}
        )
    endif()
endif()
target_compile_definitions(Test_1
    PRIVATE CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/share/config.txt"
)
target_link_libraries(Test_1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Test_1)
endif()
set_target_properties(Test_1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Test_1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Test_1)
endif()
