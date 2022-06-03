# 音视频播放器


## 目录结构
```markdown
├─CMakeLists.txt
├─main.qml                    //前端主界面
├─utils
|   ├─bufferqueue.h
|   ├─cursorposprovider.h     //全局鼠标类，提供给qml界面使用
|   ├─pcmdata.h
|   └yuvdata.h
├─src                         //后端源程序
|  ├─main.cpp                 //主程序
|  ├─stdafx.h                 //预编译头文件
|  ├─XMediaManager.cpp
|  ├─XMediaManager.h
|  ├─Video                     //视频处理
|  |   ├─SynModule.cpp
|  |   ├─SynModule.h
|  |   ├─XVideoThread.cpp
|  |   └XVideoThread.h
|  ├─Subtitle                  //字幕处理
|  |    ├─XSubtitleThread.cpp
|  |    └XSubtitleThread.h
|  ├─OpenglItem                //渲染组件，提供给qml调用
|  |     ├─myitem.cpp
|  |     ├─myitem.h
|  |     ├─myrender.cpp        //渲染器，用于接收yuv数据，绘制画面
|  |     └myrender.h
|  ├─Demux                     //解复用
|  |   ├─XDemux.cpp
|  |   ├─XDemux.h
|  |   ├─XDemuxThread.cpp
|  |   └XDemuxThread.h
|  ├─Decode                    //解码
|  |   ├─XDecode.cpp
|  |   ├─XDecode.h
|  |   ├─XDecodeThread.cpp
|  |   └XDecodeThread.h
|  ├─DataManager               //数据管理文件夹，存储数据类，数据管理类
|  |      ├─data.cpp
|  |      ├─data.h
|  |      ├─datamanager.cpp
|  |      ├─datamanager.h
|  |      ├─randomlist.cpp
|  |      └randomlist.h
|  ├─Controller                //控制器，连接前后端交互，提供前后端接口
|  |     ├─controller.cpp
|  |     ├─controller.h
|  |     └PlayMode.h
|  ├─Audio                     //音频处理
|  |   ├─audioPlay2.cpp
|  |   ├─audioPlay2.h
|  |   ├─XAudioResample.cpp
|  |   ├─XAudioResample.h
|  |   ├─XAudioThread.cpp
|  |   └XAudioThread.h
├─images                       //图片静态资源文件夹
├─ffmpeg                       //FFmpeg资源
|   ├─bin.zip
|   ├─lib
|   ├─include
├─images                      //图片静态资源文件夹
├─ffmpeg                      //ffmpeg资源文件夹
|   ├─bin.zip
|   ├─lib
|   ├─include
├─components                  //qml组件文件夹
|     ├─CoverPage.qml
|     ├─DelDialog.qml
|     ├─ErrorPopup.qml
|     ├─Footer.qml
|     ├─InfoPopup.qml
|     ├─LeftMenu.qml
|     ├─MusicView.qml
|     ├─PlayQueue.qml
|     ├─SearchBox.qml
|     ├─TitleBar.qml
|     ├─VideoPage.qml
|     └VideoView.qml
```