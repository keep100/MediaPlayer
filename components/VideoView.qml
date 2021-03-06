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
    property bool isSearched: false                //是否在搜索状态
    property variant searchList: []                //搜索结果列表

    Component {//视频列表子项
        id: videoDelegate
        Item {
            id:videoCell
            width: grid.cellWidth
            height: grid.cellHeight

            property bool isEnter: false

            Column {
                anchors.fill: parent
                anchors.leftMargin: titleBar.isMaximized||isFullSreen?50:30
                anchors.bottomMargin: titleBar.isMaximized||isFullSreen?25:15
                Rectangle{
                    width: parent.width
                    height: parent.height*0.8
                    color: "transparent"
                    border.color: videoCell.isEnter?setColor(23, 156, 241):"transparent"
                    border.width: 2
                    anchors.horizontalCenter: parent.horizontalCenter

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true

                        Image {//视频第一帧图片
                            source: "file:///"+modelData.imgPath
                            width: parent.width-2
                            height: parent.height-2
                            anchors.centerIn: parent
                            Image {//播放图标
                                source: "qrc:/images/视频播放.png"
                                anchors.centerIn: parent
                                width: 50
                                height: 50
                                visible: videoCell.isEnter
                            }
                            Image {//删除图标
                                source: "qrc:/images/删除.png"
                                anchors.right: parent.right
                                anchors.top: parent.top
                                width: 24
                                height: 24
                                visible: videoCell.isEnter

                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        delDialog.delIdx=modelData.index;
                                        delDialog.open();
                                    }
                                }
                            }
                            Rectangle{//更多信息框
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                width: 23
                                height: 23
                                radius: 100
                                color: "transparent"
                                border.width: 1
                                border.color: "white"
                                visible: videoCell.isEnter
                                Image {
                                    source: "qrc:/images/more.png"
                                    width: 18
                                    height: 18
                                    anchors.centerIn: parent
                                }
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        morePopup.file=modelData;
                                        morePopup.open();
                                    }
                                }
                            }
                        }

                        onEntered: videoCell.isEnter=true
                        onExited: videoCell.isEnter=false
                        onClicked: {
                            if(isAudioPlay){
                                isAudioPlay=false;
                                isPlaying=false;
                                controller.exit();
                            }
//                            videoPage.visible=true;
//                            mainWindow.visible=false;
                            curMediaIdx=modelData.index;
                            controller.startPlay(modelData.index,false);
//                            isVideoPlay=true;
//                            isPlaying=true;
                        }
                    }
                }

                Text {//视频名称
                    text: modelData.fileName
                    width: parent.width
                    clip: true
                    elide: Text.ElideRight
                    font.pixelSize:titleBar.isMaximized||isFullSreen?16:14
                    topPadding: 8
                    color: "white"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    //删除对话框
    DelDialog{id:delDialog;mediaType: '视频'}

    //详细信息弹窗
    InfoPopup{id:morePopup;mediaType: '视频'}

    //网格布局
    GridView {
        id: grid
        anchors.fill: parent
        cellWidth:  titleBar.isMaximized||isFullSreen?240:180
        cellHeight: cellWidth*0.7
        clip: true
        visible: model?.length>0

        model: isSearched?searchList:dataMgr?.videoList
        delegate: videoDelegate
    }

    Column{
        anchors.centerIn: parent
        visible: grid.model?.length===0

        Image {
            width: 110
            height: 110
            source: "qrc:/images/暂无视频.png"
        }
        Text {
            text: qsTr("暂无视频")
            color: "white"
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}

