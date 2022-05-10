import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//播放队列
Rectangle{
    id:playQueue
    anchors.top: parent.top
    height: parent.height-footer.height
    width: parent.width*0.22
//    x:footer.isShowQueue?windowWidth-width:windowWidth
    color: setColor(0,0,0,0.7)

    function reset(){
        queue.currentIndex=-1;
    }

    Column{
        anchors.fill: parent

        Text {
            text: qsTr("播放队列")
            width: parent.width
            leftPadding: 15
            topPadding: 30
            bottomPadding: 40
            color: "white"
            font.pixelSize: 24
            MouseArea{//防止触发到播放队列背后层级的鼠标事件
                anchors.fill: parent
                onClicked: {}
            }
        }

        //列表子项
        Component{
            id: queueDelegate
            Item {
                id:queueItem
                width: queue.width
                height: 28

                property bool isEnter: false

                Rectangle{
                    anchors.fill: parent
                    color: queueItem.isEnter?setColor(23, 66, 102):"transparent"

                    Text{
                        leftPadding: 10
                        text:videoName
                        font.pixelSize: 14
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text{
                        rightPadding: 10
                        anchors.right: parent.right
                        text:duration
                        font.pixelSize: 14
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: queueItem.isEnter=true
                    onExited: queueItem.isEnter=false
                    onClicked: queue.currentIndex=index
                }
            }
        }

        ListView{
            id:queue
            width: parent.width
            height: parent.height*0.8
            visible: true
            clip:true
            model: ListModel{
                ListElement{
                    videoName:'视频1.mp4'
                    duration:'00:46:05'
                }
                ListElement{
                    videoName:'视频1.mp4'
                    duration:'00:46:05'
                }
                ListElement{
                    videoName:'视频1.mp4'
                    duration:'00:46:05'
                }
                ListElement{
                    videoName:'视频1.mp4'
                    duration:'00:46:05'
                }
            }
            delegate: queueDelegate
            highlight: Rectangle { color: setColor(23, 66, 102)}
            Component.onCompleted: queue.currentIndex=-1
        }

        //队列空状态
        Column{
            topPadding: parent.height*0.2
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false

            Image {
                width: 120
                height: 120
                source: "qrc:/images/无内容.png"
            }
            Text {
                text: qsTr("播放队列为空")
                color: "white"
                font.pixelSize: 16
                topPadding: 15
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
