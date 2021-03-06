﻿cmake_minimum_required(VERSION 3.14)

project(MediaPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

#提供位置定位.cmake文件
set(Qt6_DIR C:/Qt/Qt6/6.3.0/msvc2019_64/lib/cmake/Qt6)

find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)

find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib)
link_libraries(avcodec avdevice avfilter avformat avutil postproc swscale swresample)

set(PROJECT_SOURCES
    src/main.cpp
    qml.qrc
    image.qrc
    logo.rc

    src/Demux/XDemux.h src/Demux/XDemux.cpp
    src/Demux/XDemuxThread.h src/Demux/XDemuxThread.cpp
    src/Decode/XDecode.h src/Decode/XDecode.cpp
    src/Decode/XDecodeThread.h src/Decode/XDecodeThread.cpp

    src/Audio/XAudioThread.h src/Audio/XAudioThread.cpp
    src/Audio/XAudioResample.h src/Audio/XAudioResample.cpp
    src/Audio/audioPlay2.h src/Audio/audioPlay2.cpp
    src/Video/XVideoThread.h src/Video/XVideoThread.cpp
    src/Video/SynModule.h src/Video/SynModule.cpp
    src/Subtitle/XSubtitleThread.h src/Subtitle/XSubtitleThread.cpp

    src/Controller/PlayMode.h
    src/Controller/controller.h  src/Controller/controller.cpp
    src/XMediaManager.h src/XMediaManager.cpp
    src/OpenglItem/myitem.h src/OpenglItem/myitem.cpp
    src/OpenglItem/myrender.h src/OpenglItem/myrender.cpp
    src/DataManager/data.h src/DataManager/data.cpp
    src/DataManager/datamanager.h src/DataManager/datamanager.cpp
    src/DataManager/randomlist.h src/DataManager/randomlist.cpp
    src/stdafx.h

    utils/cursorposprovider.h
    utils/bufferqueue.h
    utils/yuvdata.h
    utils/pcmdata.h
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MediaPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MediaPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MediaPlayer SHARED
            ${PROJECT_SOURCES}
        )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
    add_executable(MediaPlayer
        ${PROJECT_SOURCES}
    )
endif()
endif()


target_precompile_headers(
    MediaPlayer
    PUBLIC src/stdafx.h
)

target_compile_definitions(MediaPlayer
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(MediaPlayer
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

target_link_libraries(MediaPlayer PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(MediaPlayer PRIVATE Qt6::OpenGL)


target_link_libraries(MediaPlayer PRIVATE Qt6::Multimedia)
target_link_libraries(MediaPlayer PRIVATE Qt6::MultimediaWidgets)

set_target_properties(MediaPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(MediaPlayer)
    qt_finalize_executable(MediaPlayer)
endif()
