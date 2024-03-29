cmake_minimum_required(VERSION 3.5)

project(Test_1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(Python3_INCLUDE_DIRS "E:/Tools/anaconda3/envs/py311/include")
set(Python3_LIBRARIES "E:/Tools/anaconda3/envs/py311/libs/python311.lib")

include_directories(${Python3_INCLUDE_DIRS})

set(pkgdatadir ${CMAKE_CURRENT_SOURCE_DIR}/share)
set(top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(
    -DABS_TOP_SRCDIR="${top_srcdir}"
    -DDATADIR="${pkgdatadir}"
    -DNDEBUG
)

add_subdirectory(mohex)
include_directories(mohex)

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
    ai_mcts/.i
    ai_mcts/.puct
)

set(UI
    hex_ui/MainWindow.h
    hex_ui/MainWindow.cpp
    hex_ui/StartWidget.h
    hex_ui/StartWidget.cpp
    hex_ui/StartWidget.ui
    hex_ui/FuncWidget.h
    hex_ui/FuncWidget.cpp
    hex_ui/FuncWidget.ui

    hex_ui/LogWidget.h
    hex_ui/LogWidget.cpp
    hex_ui/LogWidget.ui
    hex_ui/HexLog.h
    hex_ui/HexLog.cpp
    hex_ui/HexDock.h
    hex_ui/HexDock.cpp
)

set(MODEL_SOURCES
    hex_model/HexAI.hpp
    hex_model/HexAI.cpp

    hex_model/HexIterator.hpp
    hex_model/HexArrayT.hpp
    hex_model/HexCell.hpp
    hex_model/HexLocation.hpp
    hex_model/HexCellArray.hpp
    hex_model/HexMatrix.hpp
    hex_model/HexMatrix.cpp
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

    ai_mcts/.i/AI_Mcts_I.h
    ai_mcts/.i/AI_Mcts_I.cpp
    ai_mcts/.i/MctsWork_I.h
    ai_mcts/.i/MctsWork_I.cpp
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



        ai_mcts/.i/MoWork.h ai_mcts/.i/MoWork.cpp
        ai_mcts/.i/MctsNode_I.hpp
        ai_mcts/.puct/PuctNode.hpp ai_mcts/.puct/PuctNode.cpp
        ai_mcts/.puct/nn.hpp
        ai_mcts/.puct/Mcts_puct.hpp ai_mcts/.puct/Mcts_puct.cpp


        ai_mcts/.puct/AI_AlphaZero.hpp ai_mcts/.puct/AI_AlphaZero.cpp
        ai_mcts/.puct/Py_Mcts.hpp
        ai_mcts/.puct/Py_Mcts.cpp
        ai_mcts/.puct/ShareData.h
        ai_mcts/.puct/PyThreadStateLock.hpp
        hex_ui/HexTimer.h hex_ui/HexTimer.cpp
        hex_ui/GameRecord.h hex_ui/GameRecord.cpp
        hex_ui/RecordDialog.h hex_ui/RecordDialog.cpp hex_ui/RecordDialog.ui
        ai_mcts/.puct/PMoWork.h ai_mcts/.puct/PMoWork.cpp
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
target_compile_definitions(Test_1 PRIVATE
    CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/share/config.txt"
    PY_PATH="${CMAKE_CURRENT_SOURCE_DIR}/py"
    AZA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/py/battle"
    PY_WORK_TXT="${CMAKE_CURRENT_BINARY_DIR}/working.txt"
    RECORDS_HOME="${CMAKE_CURRENT_SOURCE_DIR}/game_records"
)
target_link_libraries(Test_1 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${Python3_LIBRARIES}
)
target_link_libraries(Test_1 PUBLIC
    mohex
)

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
