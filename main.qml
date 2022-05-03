import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import "./components/"

Window {
    id:mainWindow
    width: windowWidth
    height: windowHeight
    minimumWidth: 800
    minimumHeight: 500
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinimizeButtonHint //除去窗口原生标题栏

    property int windowWidth: Screen.desktopAvailableWidth*0.65  //窗口宽度，跟随电脑屏幕变化
    property int windowHeight: Screen.desktopAvailableHeight*0.8  //窗口高度，跟随电脑屏幕变化
    property bool isFullSreen: false  //是否已经全屏
    property bool isAudioPlay: false  //是否有音频播放
    property bool isPlaying: false    //音视频是否正在播放
    property bool isCoverShow: false  //音频封面页是否已经展示
    property int curIdx: 0  //当前页面，0代表视频页面，1代表音频页面

    //设置color
    function setColor(r,g,b,a=1){
        return Qt.rgba(r/255,g/255,b/255,a)
    }
    //实现窗口最大化
    function showMax(){
        showMaximized();
        windowWidth=Screen.desktopAvailableWidth;
        windowHeight=Screen.desktopAvailableHeight;
    }
    //实现窗口正常化
    function showInitial(){
        showNormal();
        windowWidth=Screen.desktopAvailableWidth*0.65;
        windowHeight=Screen.desktopAvailableHeight*0.8;
    }
    //实现窗口全屏
    function showFull(){
        showFullScreen();
        windowWidth=Screen.width;
        windowHeight=Screen.height;
    }

    //监听是否有音视频在播放
    onIsAudioPlayChanged: {
        if(!isAudioPlay){
            musicList.currentIndex=-1;
            queue.currentIndex=-1;
        }
    }

    //窗口背景
    Image {
        id: bg
        source: "./images/bg.jpg"
        width: windowWidth
        height: windowHeight
        smooth: true
    }

    //左侧导航栏
    LeftMenu{id:leftMenu}

    //内容展示区
    Rectangle{
        id:contentView
        anchors.top: titleBar.top
        anchors.left: leftMenu.right
        anchors.right: parent.right
        anchors.bottom: footer.top
        color: setColor(25, 25, 25,0.6)

        Row{
            id:titleRow
            anchors.left: parent.left
            leftPadding: 30
            topPadding: 50
            width: parent.width

            //当前页面标题
            Label{
                text: curIdx===0?'视频':'音乐'
                font.pixelSize: 24
                color: "white"
            }

            //搜索框
            SearchBox{id:searchBox}
        }

        //上传文件按钮
        Button{
            id: uploadBtn
            x: titleRow.x+30
            y:titleRow.y+titleRow.height+8
            text: curIdx===0?"导入视频":"导入音频"
            hoverEnabled:false

            contentItem: Text {
                text: uploadBtn.text
                font.pixelSize: 12
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                implicitWidth: 80
                implicitHeight: 25
                color: uploadBtn.down?setColor(97, 173, 253):setColor(77, 153, 252)
                radius: 20
            }
        }

        //视频数据列表
        //        VideoListData {id:videoData }

        //视频展示区
        VideoView{id:videoView}

        //音频展示区
        MusicView{id:musicView}
    }

    //标题栏
    TitleBar{id:titleBar}

    //音乐封面页面
    CoverPage{id:coverPage}

    //音频封面滑动动画
    PropertyAnimation{
        id:coverAnimation
        target: coverPage
        properties: "y"
        duration: 300
        easing.type: Easing.InQuad
    }

    //播放队列
    PlayQueue{id:playQueue}

    //播放队列滑动动画
    PropertyAnimation{
        id: queueAnimation
        target: playQueue
        properties: "x"
        duration: 800
        easing.type: Easing.InOutExpo //渐变滑出效果
    }

    //播放进度条
    Slider {
        id: mySlider
        value: 0.2
        y:footer.y-mySlider.availableHeight / 2
        z:1
        visible: isAudioPlay

        background: Rectangle {
            y: mySlider.availableHeight / 2 - height / 2
            implicitWidth: windowWidth
            implicitHeight: 4
            width: mySlider.availableWidth
            height: implicitHeight
            radius: 2
            color: "white"

            Rectangle {
                width: mySlider.visualPosition * parent.width
                height: parent.height
                color: setColor(22, 132, 252)
                radius: 2
            }
        }

        handle: Rectangle {
            x:  mySlider.visualPosition * (mySlider.availableWidth - width)
            y:  mySlider.availableHeight / 2 - height / 2
            implicitWidth: 10
            implicitHeight: 10
            radius: 100
            color: mySlider.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }
    }

    //底部控制栏
    Footer{id:footer}
}
