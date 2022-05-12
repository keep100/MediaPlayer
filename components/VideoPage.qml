import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Window {
    id:videoPage
    width: windowWidth
    height: windowHeight
    visible: false
    flags: Qt.Window | Qt.FramelessWindowHint  //除去窗口原生标题栏

    property bool isEnabled: false    //是否启用底部栏动画
    property bool isShowQueue: false  //是否展示了视频播放列表

    //定时器，时间到隐藏顶部和底部栏
    Timer {
        id:timer
        interval: 3000
        running: videoPage.visible
        onTriggered: {
            topBar.y=-topBar.height;
            footer.y=windowHeight;
        }
    }

    //控制底部栏动画启用的定时器
    Timer {
        id:timer1
        interval: 10
        running: videoPage.visible&&isFullSreen
        onTriggered: isEnabled=true;
    }

    //视频流展示区域
    Image {
        id:img
        anchors.fill: parent
        cache: false
        mipmap: true
        smooth: true
//        fillMode: Image.PreserveAspectFit
    }

    //与ShowImage类实例建立连接，接收刷新界面的信号
    Connections{
        target: MyImage
        function onCallQmlRefeshImg(){
            img.source = "";
            img.source = "image://Imgs";
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        //监听鼠标移动
        onPositionChanged: {
            topBar.y=0;
            footer.y=windowHeight-footer.height
            timer.stop();
            timer.interval=5000;
            timer.start();
        }

        //顶部栏
        Rectangle{
            id:topBar
            width: parent.width
            height: 35
            color: setColor(0,0,0,0.6)
            Row{
                anchors.fill: parent
                leftPadding: 10
                Image {
                    source: "qrc:/images/logo.png"
                    width: parent.height*0.9
                    height: width
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text: qsTr("播放器名字")
                    leftPadding: 8
                    color: "white"
                    font.pixelSize: 14
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text: qsTr("这是视频名字")
                    leftPadding: 10
                    color: "white"
                    font.pixelSize: 14
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton //只处理鼠标左键
                property point clickPos: "0,0"    //鼠标点击标题栏时的坐标

                //鼠标按下的回调函数
                onPressed: function(mouse) {
                    clickPos = Qt.point(mouse.x,mouse.y)
                }

                //鼠标挪动的回调函数
                onPositionChanged:function (mouse) {
                    //判断当前窗口是否在正常化状态，是就直接移动窗口位置
                    if(pressed && !titleBar.isMaximized&&!isFullSreen) {
                        videoPage.setX(mousePosition.cursorPos().x-clickPos.x);
                        videoPage.setY(mousePosition.cursorPos().y-clickPos.y);
                    }
                }
            }

            Behavior on y {
                NumberAnimation { duration: 500;easing.type: Easing.InOutExpo }
            }
        }

        //视频播放队列
        PlayQueue{
            id:videoQueue
            x:isShowQueue?windowWidth-videoQueue.width:windowWidth
        }

        //底部控制栏
        Footer{
            id:footer
            y:windowHeight-footer.height

            Behavior on y {
                enabled: !isFullSreen||isEnabled
                NumberAnimation { duration: 500;easing.type: Easing.InOutExpo }
            }
        }
    }
}
