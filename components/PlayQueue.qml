import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//播放队列
Rectangle{
    id:playQueue
    anchors.top: parent.top
    height: parent.height-footer.height
    width: parent.width*0.22
    color: setColor(0,0,0,0.7)

    function setIdx(idx){
        queue.currentIndex=idx;
    }
    //关闭播放队列
    function onClose(){
        mainWindow.isShowQueue=false;
        queueAnimation.from = windowWidth-playQueue.width;
        queueAnimation.to = windowWidth;
        queueAnimation.target=playQueue;
        queueAnimation.running = true;
    }
    //打开播放界面
    function openVideoPage(){
        videoPage.visible=true;
        mainWindow.visible=false;
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
                        text:modelData.fileName
                        width: queue.width*0.6
                        clip: true
                        elide: Text.ElideRight
                        font.pixelSize: 14
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text{
                        rightPadding: 10
                        anchors.right: parent.right
                        text: formatTime(modelData.duration)
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
                    onClicked:{
                        if(curMediaIdx!==modelData.index){
                            curMediaIdx=modelData.index;
                            if(curIdx===0){
                                if(isAudioPlay){
                                    isAudioPlay=false;
                                    isPlaying=false;
                                    controller.exit();
                                }
                                if(mainWindow.visible){
                                    onClose();
                                    openVideoPage();
                                }
                            }
                            controller.startPlay(modelData.index,curIdx===1);
                            curIdx?isAudioPlay=true:isVideoPlay=true;
                            isPlaying=true;
                        }
                    }
                }
            }
        }

        ListView{
            id:queue
            width: parent.width
            height: parent.height*0.8
            visible: true
            clip:true
            model: mainWindow.curIdx? dataMgr?.audioList: dataMgr?.videoList
                                                                   delegate: queueDelegate
            highlight: Rectangle { color: setColor(23, 66, 102)}
            //列表数据改变
            onModelChanged: isAudioPlay||isVideoPlay?
                                (isAudioPlay?queue.currentIndex=dataMgr?.curAudio.index ?? 0
                                                                                         :queue.currentIndex=dataMgr?.curVideo.index ?? 0)
                              :setIdx(-1)
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
