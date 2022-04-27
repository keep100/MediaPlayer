import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Rectangle{
    id:titleBar
    anchors.left: parent.left
    anchors.right: parent.right
    height: 35
    color: "transparent"

    property bool isDoubleClicked: false //判断标题栏是否被双击，当isDoubleClicked为true时最大化窗口，反之正常化窗口
    property bool isMaximized: false //判断当前是否最大化
    property point clickPos: "0,0" //鼠标点击标题栏时的坐标

    Connections{
        target: mainWindow
        function onVisibilityChanged() {//解决Qt窗口最大化的时候最小化，再恢复窗口变为普通窗口的bug
            if(isMaximized&&mainWindow.visibility===2){
                mainWindow.showMax();
            }
        }
    }

    //关闭窗口按钮
    Button{
        id:closeBtn
        anchors.right: parent.right
        width: parent.height
        height: parent.height
        hoverEnabled : true
        z:1

        //自定义按钮背景
        background: Rectangle{
            color: closeBtn.hovered ? "red" : "transparent"
            Image{
                width: parent.width*0.5
                height: parent.height*0.5
                anchors.centerIn: parent
                source: "file:///C:/Users/16321/Pictures/关闭.png"
            }
        }
        onClicked: {
            mainWindow.close();
        }
    }

    //最大化或正常化按钮
    Button{
        id: normalBtn
        anchors.right: closeBtn.left
        width: parent.height
        height: parent.height
        hoverEnabled : true
        z:1

        //自定义按钮背景
        background: Rectangle{
            color: normalBtn.hovered ? (normalBtn.pressed ? "#4E4E4E" : "#666666") : "transparent"
            Image{
                width: parent.height * 0.4
                height: parent.height * 0.4
                anchors.centerIn: parent
                source: isMaximized?
                            "file:///C:/Users/16321/Pictures/正常化.png":
                            "file:///C:/Users/16321/Pictures/最大化.png"
            }
        }
        onClicked:{
            if(isMaximized){
                isMaximized = false;
                mainWindow.showInitial();
            }else{
                isMaximized = true;
                mainWindow.showMax();
            }
        }
    }

    //最小化按钮
    Button{
        id: miniBtn
        anchors.right: normalBtn.left
        width: parent.height
        height: parent.height
        hoverEnabled : true
        z:1

        //自定义按钮背景
        background: Rectangle{
            color: miniBtn.hovered ? (miniBtn.pressed ? "#4E4E4E" : "#666666") : "transparent"
            Image{
                width: parent.height * 0.5
                height: parent.height * 0.5
                anchors.centerIn: parent
                source: "file:///C:/Users/16321/Pictures/最小化.png"
            }
        }
        onClicked:{
            mainWindow.showMinimized();
        }
    }


    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton //只处理鼠标左键

        //鼠标按下的回调函数
        onPressed: function(mouse) {
            clickPos = Qt.point(mouse.x,mouse.y)
        }

        //鼠标按下并挪动的回调函数
        onPositionChanged:function (mouse) {
            //判断当前窗口是否在正常化状态，是就直接移动窗口位置
            if(pressed && mainWindow.visibility !== Window.Maximized && mainWindow.visibility !== Window.FullScreen) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                mainWindow.x += delta.x
                mainWindow.y += delta.y
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
            if(isDoubleClicked){
                mainWindow.showMax();
            }else{
                mainWindow.showInitial();
            }
        }
    }
}
