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
//    Image {
//        id:img
//        anchors.fill: parent
//        cache: false
//        mipmap: true
//        smooth: true
////        fillMode: Image.PreserveAspectFit
//    }

    //与ShowImage类实例建立连接，接收刷新界面的信号
//    Connections{
//        target: MyImage
//        function onCallQmlRefeshImg(){
//            img.source = "";
//            img.source = "image://Imgs";
//        }
//    }

    //加载页面
    Rectangle{
        anchors.fill: parent
        color: setColor(13, 18, 31)
        Label{
            anchors.centerIn: parent
            text: '玩命加载中 . . .'
            font.pixelSize: 16
            color: "white"
        }
    }

    //错误信息对话框
    Dialog {
        id: messageDialog
        modal: true
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose

        background: Rectangle{
            anchors.fill: parent
            color:setColor(29, 34, 36)
        }

        header: Row{//对话框头部
            leftPadding: 15
            topPadding: 15
            bottomPadding: 10
            rightPadding: 10
            Label{
                text: '该视频无法正常播放，请选择以下操作:'
                font.pixelSize: 15
                rightPadding: 40
                color: "white"
            }
//            Rectangle{
//                id:closeBtn
//                color: isEnter?setColor(247, 68, 68):"transparent"
//                width: 18
//                height: 18
//                anchors.verticalCenter: parent.verticalCenter
//                property bool isEnter: false

//                Image {
//                    source: "qrc:/images/关闭.png"
//                    width: 13
//                    height: 13
//                    anchors.centerIn: parent
//                    MouseArea{
//                        anchors.fill: parent
//                        hoverEnabled: true
//                        onEntered: closeBtn.isEnter=true
//                        onExited: closeBtn.isEnter=false
//                        onClicked: delDialog.close()
//                    }
//                }
//            }
        }

        footer: Row{//对话框底部按钮区域
            layoutDirection:Qt.RightToLeft
            spacing: 15
            rightPadding: 10
            bottomPadding: 10

            //退出播放界面
            Button{
                hoverEnabled:false
                property bool isHover: false

                contentItem: Text {
                    text: "退出播放"
                    font.pixelSize: 12
                    color:  parent.isHover? "white" : setColor(17, 204, 253)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 28
                    color: parent.isHover?setColor(25, 116, 185):setColor(30, 45, 56)
                    radius: 4
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.isHover=true
                    onExited: parent.isHover=false
                    onClicked: {
                        if(isVideoPlay){
                            isVideoPlay=false;
                            isPlaying=false;
                        }
                        messageDialog.close();
                        mainWindow.visible=true;
                        videoPage.isShowQueue=false;
                        videoPage.close();
                    }
                }
            }

            //播放下一个视频
            Button{
                hoverEnabled:false
                property bool isHover: false

                contentItem: Text {
                    text: "下一首"
                    font.pixelSize: 12
                    color:  parent.isHover? "white" : setColor(17, 204, 253)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 28
                    color: parent.isHover?setColor(25, 116, 185):setColor(30, 45, 56)
                    radius: 4
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.isHover=true
                    onExited: parent.isHover=false
                    onClicked: {
                        messageDialog.close()
                    }
                }
            }

            //删除该视频记录并播放下一首
            Button{
                hoverEnabled:false
                property bool isHover: false

                contentItem: Text {
                    text: "移除该视频"
                    font.pixelSize: 12
                    color:  parent.isHover? "white" : setColor(17, 204, 253)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 28
                    color: parent.isHover?setColor(25, 116, 185):setColor(30, 45, 56)
                    radius: 4
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.isHover=true
                    onExited: parent.isHover=false
                    onClicked: {
                        messageDialog.close()
                    }
                }
            }
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

        onClicked: messageDialog.open()
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

        //视频播放进度条
        Slider {
            id: videoSlider
            value: 0.2
            y:footer.y-videoSlider.availableHeight / 2
            z:1
            visible: footer.y!==windowHeight

            background: Rectangle {
                y: videoSlider.availableHeight / 2 - height / 2
                implicitWidth: windowWidth
                implicitHeight: 4
                width: videoSlider.availableWidth
                height: implicitHeight
                radius: 2
                color: "white"

                Rectangle {
                    width: videoSlider.visualPosition * parent.width
                    height: parent.height
                    color: setColor(22, 132, 252)
                    radius: 2

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            videoSlider.background.implicitHeight=6;
                            videoSlider.handle.visible=true;
                        }

                        onExited: {
                            videoSlider.background.implicitHeight=4;
                            videoSlider.handle.visible=false;
                        }
                    }
                }
            }

            handle: Rectangle {
                x:  videoSlider.visualPosition * (videoSlider.availableWidth - width)
                y:  videoSlider.availableHeight / 2 - height / 2
                implicitWidth: 10
                implicitHeight: 10
                radius: 100
                color: videoSlider.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
                visible: false
            }
        }

        //底部控制栏
        Footer{
            id:footer
            y:windowHeight-footer.height
            mediaType:'video'

            Behavior on y {
                enabled: !isFullSreen||isEnabled
                NumberAnimation { duration: 500;easing.type: Easing.InOutExpo }
            }
        }
    }
}
