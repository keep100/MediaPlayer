cmake_minimum_required(VERSION 3.14)

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
link_libraries(avcodec avformat avutil swscale swresample)

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
        image.qrc
        XDemux.h XDemux.cpp
        XDecode.h XDecode.cpp
        XVideoWidget.h XVideoWidget.cpp
        XResample.h XResample.cpp
        XAudioPlay.h XAudioPlay.cpp
        XAudioThread.h XAudioThread.cpp
        XVideoThread.h XVideoThread.cpp
        IVideoCall.h
        xdemuxthread.h xdemuxthread.cpp
        imageprovider.h imageprovider.cpp
        cursorposprovider.h
        XDecodeThread.h XDecodeThread.cpp
        OverloadSlider.h OverloadSlider.cpp
        widget.h widget.cpp widget.ui
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
