import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//视频展示区
Rectangle {
    id:videoView
    y:uploadBtn.y+uploadBtn.height+20
    width: parent.width
    height: parent.height-y
    color: "transparent"
    visible: curIdx===0

    Component {//视频列表子项
        id: videoDelegate
        Item {
            width: grid.cellWidth
            height: grid.cellHeight
            Column {
                anchors.fill: parent
                anchors.leftMargin: titleBar.isMaximized||isFullSreen?50:30
                anchors.bottomMargin: titleBar.isMaximized||isFullSreen?25:15
                Image {
                    width: parent.width
                    height: parent.height*0.8
                    source: src
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: videoName
                    font.pixelSize:titleBar.isMaximized||isFullSreen?16:14
                    topPadding: 8
                    color: "white"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    //视频数据列表
    VideoListData {id:videoData }

    //网格布局
    GridView {
        id: grid
        anchors.fill: parent
        cellWidth:  titleBar.isMaximized||isFullSreen?240:180
        cellHeight: cellWidth*0.7
        clip: true
        visible: videoData.count>0

        model:videoData
        delegate: videoDelegate
    }

    Column{
        anchors.centerIn: parent
        visible: videoData.count===0

        Image {
            width: 110
            height: 110
            source: "../images/暂无视频.png"
        }
        Text {
            text: qsTr("暂无视频")
            color: "white"
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}

