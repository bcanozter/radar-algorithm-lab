cmake_minimum_required(VERSION 3.16)
project(radar_algorithm_lab VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL "")
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.11.1/gcc_64")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort)

qt_standard_project_setup()

qt_add_executable(radar_algorithm_lab
    # core library
    src/core/ConfigPort.cpp
    src/core/ConfigPort.h
    src/core/DataPort.cpp
    src/core/DataPort.h
    src/core/SerialPort.cpp
    src/core/SerialPort.h
    src/core/TlvTypes.h
    src/core/DataParser.cpp
    src/core/DataParser.h

    # gui
    src/gui/main.cpp
    src/gui/MainWindow.cpp
    src/gui/MainWindow.h
    src/gui/ConfigPanel.cpp
    src/gui/ConfigPanel.h
    src/gui/LogView.cpp
    src/gui/LogView.h
    src/gui/ConnectDialog.cpp
    src/gui/ConnectDialog.h
)

target_link_libraries(radar_algorithm_lab PRIVATE Qt6::Core Qt6::Widgets Qt6::SerialPort)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_MESSAGELOGCONTEXT)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_DEBUG_OUTPUT)
endif()
