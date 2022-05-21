# 音视频播放器


## 目录结构
```markdown
├─until
|   ├─cursorposprovider.h     //全局鼠标类，提供给qml界面使用
|   ├─imageprovider.cpp       //图片加载类，提供给qml界面的Image组件，使其能否加载内存中的QImage
|   └imageprovider.h
├─OpenglItem
|     ├─myitem.cpp            //渲染组件，提供给qml调用
|     ├─myitem.h      
|     ├─myrender.cpp          //渲染器，用于接收yuv数据，绘制画面
|     └myrender.h
├─images                      //图片静态资源文件夹
├─ffmpeg                      //ffmpeg资源文件夹
|   ├─bin.zip
|   ├─lib
|   ├─include
├─DataManager                 //数据管理文件夹，存储数据类，数据管理类
|      ├─data.cpp
|      ├─data.h
|      ├─datamanager.cpp
|      ├─datamanager.h
|      ├─randomlist.cpp
|      └randomlist.h
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