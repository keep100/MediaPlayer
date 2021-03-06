import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Rectangle{
    id:titleBar
    anchors.left: parent.left
    anchors.right: parent.right
    height: 35
    color: "transparent"
    visible: !isFullSreen

    property bool isDoubleClicked: false //判断标题栏是否被双击，当isDoubleClicked为true时最大化窗口，反之正常化窗口
    property bool isMaximized: false //判断当前是否最大化

    Connections{
        target: mainWindow
        function onVisibilityChanged() {//解决Qt窗口最大化的时候最小化，再恢复窗口变为普通窗口的bug
            if(isMaximized&&mainWindow.visibility===2){
                mainWindow.showMax();
            }
        }
    }

    //关闭窗口按钮
    Rectangle{
        id:closeBtn
        anchors.right: parent.right
        width: parent.height
        height: parent.height
        color: "transparent"
        radius: 10
        z:1
        Image{
            width: parent.width*0.5
            height: parent.height*0.5
            anchors.centerIn: parent
            source: "qrc:/images/关闭.png"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: closeBtn.color=setColor(255,0,0,0.6)
            onExited: closeBtn.color="transparent"
            onClicked: mainWindow.close()
        }
    }

    //最大化或正常化按钮
    Rectangle{
        id: normalBtn
        anchors.right: closeBtn.left
        width: parent.height
        height: parent.height
        color: "transparent"
        radius: 10
        z:1
        Image{
            width: parent.height * 0.4
            height: parent.height * 0.4
            anchors.centerIn: parent
            source: isMaximized?
                        "qrc:/images/正常化.png":
                        "qrc:/images/最大化.png"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: normalBtn.color=setColor(255,255,255,0.2)
            onExited: normalBtn.color="transparent"
            onClicked: {
                if(isMaximized){
                    isMaximized = false;
                    mainWindow.showInitial();
                }else{
                    isMaximized = true;
                    mainWindow.showMax();
                }
            }
        }
    }

    //最小化按钮
    Rectangle{
        id: miniBtn
        anchors.right: normalBtn.left
        width: parent.height
        height: parent.height
        color: "transparent"
        radius: 10
        z:1
        Image {//最小化图标
            width: parent.width*0.5
            height: parent.height*0.5
            anchors.centerIn: parent
            source: "qrc:/images/最小化.png"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: miniBtn.color=setColor(255,255,255,0.2)
            onExited: miniBtn.color="transparent"
            onClicked: mainWindow.showMinimized()
        }
    }

    //设置按钮
    Rectangle{
        id:configBtn
        anchors.right: miniBtn.left
        width: parent.height
        height: parent.height
        color: "transparent"
        radius: 10
        z:1
        Image {//设置图标
            width: parent.width*0.5
            height: parent.height*0.5
            anchors.centerIn: parent
            source: "qrc:/images/设置.png"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: configBtn.color=setColor(255,255,255,0.2)
            onExited: configBtn.color="transparent"
            onClicked: configDialog.open()
        }
    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton //只处理鼠标左键

        property point clickPos: "0,0" //鼠标点击标题栏时的坐标

        //鼠标按下的回调函数
        onPressed: function(mouse) {
            clickPos = Qt.point(mouse.x,mouse.y)
        }

        //鼠标按下并挪动的回调函数
        onPositionChanged:function (mouse) {
            //判断当前窗口是否在正常化状态，是就直接移动窗口位置
            if(pressed && mainWindow.visibility !== Window.Maximized && mainWindow.visibility !== Window.FullScreen) {
                mainWindow.setX(mousePosition.cursorPos().x-clickPos.x);
                mainWindow.setY(mousePosition.cursorPos().y-clickPos.y);
            }
            //当当前窗口在最大化状态下，退出最大化
            if(mainWindow.visibility === Window.Maximized && pressed)
            {
                isMaximized = false;
                isDoubleClicked=false;
                showInitial();
            }
        }

        //双击标题栏的回调函数
        onDoubleClicked: {
            isDoubleClicked=!isDoubleClicked;
            isMaximized=!isMaximized;
            isDoubleClicked?mainWindow.showMax():mainWindow.showInitial();
        }
    }
}
