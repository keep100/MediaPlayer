import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//音频展示区
Rectangle{
    id:musicView
    width: parent.width
    height: parent.height-y
    y:uploadBtn.y+uploadBtn.height+20
    color: "transparent"
    visible: curIdx===1
    property bool isSearched: false                //是否在搜索状态
    property variant searchList: []                //搜索结果列表

    function setIdx(idx){
        musicList.currentIndex=idx;
    }

    //有音频的状态展示区
    Column{
        leftPadding: uploadBtn.x
        visible: musicList.count>0
        Image {
            id:listImg
            width: 100
            height: 100
            source: "qrc:/images/音乐封面.png"
        }
        Row{
            id:titleRow
            topPadding: 10
            bottomPadding: 10
            Text {
                text: qsTr("歌曲列表")
                color: "white"
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                leftPadding: 14
                text: qsTr("共"+dataMgr?.audioList.length+"首")
                color: "white"
                font.pixelSize: 14
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        //音频列表展示区
        Column{
            width: musicView.width*0.9

            //列表表头
            Rectangle{
                id:header
                width: parent.width
                height: 36
                color: setColor(40, 41, 40,0.85)

                Text {
                    leftPadding: parent.width*0.1
                    text: qsTr("标题")
                    color: "white"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    leftPadding: parent.width*0.6
                    text: qsTr("歌手")
                    color: "white"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    leftPadding: parent.width*0.9
                    text: qsTr("时长")
                    color: "white"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            //列表子项
            Component{
                id:musicDelegate
                Item{
                    id:musicItem
                    width: musicList.width
                    height: 30
                    property bool isEnter: false

                    Rectangle{
                        anchors.fill: parent
                        color: musicItem.isEnter||musicList.currentIndex===modelData.index?
                                   setColor(66, 105, 137,0.8):setColor(75, 75, 80,0.6)

                        Image {
                            x:parent.width*0.04
                            width: 18
                            height: 18
                            source: musicList.currentIndex===modelData.index?"qrc:/images/音量.png":""
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            leftPadding: parent.width*0.1
                            text: qsTr(modelData.fileName)
                            color: "white"
                            font.pixelSize: 13
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Image {
                            x: parent.width*0.42
                            width: 14
                            height: 14
                            source: "qrc:/images/播放.png"
                            fillMode: Image.PreserveAspectFit
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    if(curMediaIdx!==modelData.index){
                                        curMediaIdx=modelData.index;
                                        controller.startPlay(modelData.index,true);
                                        isAudioPlay=true;
                                        isPlaying=true;
                                    }
                                }
                            }
                        }
                        Image {
                            x: parent.width*0.48
                            width: 16
                            height: 16
                            source: "qrc:/images/more.png"
                            fillMode: Image.PreserveAspectFit
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    morePopup.file=modelData;
                                    morePopup.open();
                                }
                            }
                        }
                        Image {
                            x: parent.width*0.53
                            width: 16
                            height: 16
                            source: "qrc:/images/删除.png"
                            fillMode: Image.PreserveAspectFit
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    delDialog.open();
                                    delDialog.delIdx=modelData.index;
                                }
                            }
                        }
                        Text {
                            leftPadding: parent.width*0.6
                            text: qsTr(modelData.artist)
                            width: parent.width*0.85
                            clip: true
                            elide: Text.ElideRight
                            color: "white"
                            font.pixelSize: 13
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            leftPadding: parent.width*0.9
                            text: qsTr(formatTime(modelData.duration))
                            color: "white"
                            font.pixelSize: 13
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        propagateComposedEvents:true
                        onEntered: musicItem.isEnter=true
                        onExited: musicItem.isEnter=false
                    }
                }
            }

            //列表内容部分
            ListView{
                id:musicList
                width: parent.width
                height:musicView.height-listImg.height-titleRow.height-header.height
                model: isSearched?searchList:dataMgr?.audioList
                clip: true
                delegate:musicDelegate
                visible: model?.length>0
                //音频数据列表改变
                onModelChanged: isAudioPlay?musicList.currentIndex=dataMgr?.curAudio.index ?? 0 :setIdx(-1)
            }
        }
    }

    //删除对话框
    DelDialog{id:delDialog;mediaType: '音频'}

    //详细信息弹窗
    InfoPopup{id:morePopup;mediaType: '音频'}

    //无音频的空状态展示区
    Column{
        anchors.centerIn: parent
        visible: musicList.model?.length===0

        Image {
            width: 100
            height: 100
            source: "qrc:/images/无音乐.png"
        }
        Text {
            topPadding: 10
            text: qsTr("暂无音频")
            color: "white"
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
