import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//底部控制栏
Rectangle{
    id:myFooter
    y:parent.height-height
    width: parent.width
    height: 40
    color: videoPage.visible?setColor(0,0,0,0.6):setColor(43, 45, 47,0.95)
    focus: true  //得获取焦点，才能监听键盘事件
    
    property variant pressedKeys: new Set()  //存储按下还未释放的按键
    required property string mediaType       //判断是否是视频底部栏

    //获取音频略缩图
    function getCover(path){
        return path? "file:///"+path : "qrc:/images/音乐封面.png"
    }
    
    Keys.onPressed:function(e) {
        //将键值加入set
        if(!e.isAutoRepeat){
            pressedKeys.add(e.key);
        }
        //启动定时器
        if(!timer.running){
            timer.start();
        }
    }
    Keys.onReleased: function(e){
        //将按键值弹出
        if(!e.isAutoRepeat){
            pressedKeys.delete(e.key);
        }
        //停止定时器
        if(pressedKeys.size<=0){
            timer.stop();
        }
    }
    
    //用于控制遍历按键set的时机
    Timer{
        id:timer
        repeat: true
        interval: 50
        onTriggered: {
            //如果没有按键按下，停止定时器
            if(pressedKeys.size<=0){
                timer.stop();
                return;
            }
            
            //遍历已经按下的按键，处理多个按键
            for(let key of pressedKeys){
                switch(key){
                case Qt.Key_Space:     //处理空格键，暂停或播放音视频
                    console.log('sapce');
                    if(isAudioPlay||isVideoPlay){
                        isPlaying=!isPlaying;
                    }
                    break;
                case Qt.Key_Escape:     //处理esc键，退出全屏
                    console.log('esc');
                    if(isFullSreen){
                        isFullSreen=false;
                        showInitial();
                    }
                    break;
                case Qt.Key_F:         //处理Ctrl+F，全屏
                    if(pressedKeys.has(Qt.Key_Control)){
                        console.log('ctrl F');
                        isFullSreen=true;
                        showFull();
                    }
                    break;
                case Qt.Key_I:         //处理Ctrl+I，唤起资源导入弹窗
                    if(pressedKeys.has(Qt.Key_Control)){
                        console.log('ctrl I');
                        if(!fileDialog.visible){
                            fileDialog.open();
                        }
                    }
                    break;
                case Qt.Key_Left:      //处理Ctrl+ ← ，上一首
                    if(pressedKeys.has(Qt.Key_Control)){
                        console.log('ctrl left');
                    }
                    break;
                case Qt.Key_Right:      //处理Ctrl+ → ，下一首
                    if(pressedKeys.has(Qt.Key_Control)){
                        console.log('ctrl right');
                    }
                    break;
                case Qt.Key_Up:      //处理Ctrl+ ↑ ，增加音量
                    if(pressedKeys.has(Qt.Key_Control)){
                        console.log('ctrl up');
                    }
                    break;
                case Qt.Key_Down:      //处理Ctrl+ ↓ ，降低音量
                    if(pressedKeys.has(Qt.Key_Control)){
                        console.log('ctrl down');
                    }
                    break;
                }
            }
        }
    }
    
    Row{
        width: parent.width
        height: parent.height
        
        //音频略缩图、歌名和歌手信息
        Row{
            leftPadding: 20
            height: parent.height
            opacity: isAudioPlay?1:0
            
            //音频略缩图
            Image {
                id: smallCover
                source: getCover(dataMgr?.curAudio.imgPath)
                width: parent.height*0.8
                height: width
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                
                //向上弹出音频封面页的图标
                Image {
                    id:upIcon
                    width: 18
                    height: 18
                    opacity: 0
                    anchors.centerIn: parent
                    source: "qrc:/images/向上.png"
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: upIcon.opacity=0.5
                    onExited: upIcon.opacity=0
                    onClicked:{
                        if(!isCoverShow){
                            isCoverShow=true;
                            coverAnimation.from=windowHeight;
                            coverAnimation.to=0;
                            coverAnimation.running=true;
                        }
                    }
                }
            }
            
            //音频名字与歌手
            Column{
                height: parent.height
                leftPadding: 8
                Text {
                    text: dataMgr?.curAudio.fileName ?? "歌名"
                    topPadding: 2
                    width: 150
                    clip: true
                    elide: Text.ElideRight
                    font.pixelSize: 13
                    color: "white"
                }
                Text {
                    text: dataMgr?.curAudio.artist ?? "歌手"
                    topPadding: 5
                    width: 150
                    clip: true
                    elide: Text.ElideRight
                    font.pixelSize: 11
                    color: "white"
                }
            }
        }

        //视频的已播放时长与总时长
        Text {
            leftPadding: -200
            text: formatTime(controller?.time)+"/"+formatTime(dataMgr?.curVideo.duration)
            font.pixelSize: 14
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            opacity: isVideoPlay? 1:0
        }
        
        //退出播放音视频的按钮
        Image {
            id: exitBtn
            source: isAudioPlay||isVideoPlay?"qrc:/images/停止.png":"qrc:/images/停止-禁用.png"
            width: 20
            height: 20
            x:parent.width*0.3
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    controller.exit();
                    if(isAudioPlay){
                        isAudioPlay=false;
                        isPlaying=false;
                    }
                    if(videoPage.visible){
                        mainWindow.visible=true;
                        videoPage.isShowQueue=false;
                        isVideoPlay=false;
                        isPlaying=false;
                        videoPage.close();
                    }
                    if(isCoverShow){
                        isCoverShow=false;
                        coverAnimation.from=0;
                        coverAnimation.to=windowHeight;
                        coverAnimation.running=true;
                    }
                }
            }
        }
        
        //上一首按钮
        Image {
            id: prevBtn
            source: isAudioPlay||isVideoPlay?"qrc:/images/上一首.png":"qrc:/images/上一首-禁用.png"
            width: 26
            height: 26
            x:exitBtn.x+windowWidth*0.08
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    controller.playPre(mediaType==='音频');
                    curMediaIdx=isAudioPlay?dataMgr.curAudio.index:dataMgr.curVideo.index;
                }
            }
        }
        
        //播放与暂停按钮
        Image {
            id: playBtn
            source: isAudioPlay||isVideoPlay?(isPlaying?"qrc:/images/pause.png":"qrc:/images/播放.png")
                                            :"qrc:/images/播放-禁用.png"
            width: 15
            height: 15
            x:prevBtn.x+windowWidth*0.07
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(isAudioPlay||isVideoPlay){
                        isPlaying=!isPlaying;
                    }
                }
            }
        }
        
        //下一首按钮
        Image {
            id: nextBtn
            source: isAudioPlay||isVideoPlay?"qrc:/images/下一首.png":"qrc:/images/下一首-禁用.png"
            width: 26
            height: 26
            x:playBtn.x+windowWidth*0.06
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    controller.playNext(mediaType==='音频');
                    curMediaIdx=isAudioPlay?dataMgr.curAudio.index:dataMgr.curVideo.index;
                }
            }
        }
        
        //音量图标
        Image {
            id: soundIcon
            source: "qrc:/images/音量.png"
            width: 16
            height: 16
            x:nextBtn.x+windowWidth*0.08
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
        }
        
        //音量控制条
        Slider {
            id: soundSlider
            from: 0
            value: mainWindow.voice
            to:100
            x:soundIcon.x+windowWidth*0.02
            pressed: false
            anchors.verticalCenter: parent.verticalCenter
            
            background: Rectangle {//控制条背景
                y: soundSlider.availableHeight / 2 - height / 2
                implicitWidth: 100
                implicitHeight: 4
                width: soundSlider.availableWidth
                height: implicitHeight
                radius: 2
                color: "white"
                
                Rectangle {//滑块左侧控制条部分背景
                    width: soundSlider.visualPosition * parent.width
                    height: parent.height
                    color: setColor(22, 132, 252)
                    radius: 2
                }
            }
            
            handle: Rectangle {//滑块
                x:  soundSlider.visualPosition * (soundSlider.availableWidth - width)
                y:  soundSlider.availableHeight / 2 - height / 2
                implicitWidth: 13
                implicitHeight: 13
                radius: 100
                color: soundSlider.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
            }
            onPressedChanged: {//监听最后释放位置
                if(!pressed){
                    mainWindow.voice=Math.floor(value);
                }
            }
        }

        //音频的已播放时长与总时长
        Text {
            id:audioTime
            x:playMode.x-width-windowWidth*0.02
            text: formatTime(controller?.time)+"/"+formatTime(dataMgr?.curAudio.duration)
            font.pixelSize: 14
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            opacity: isAudioPlay?1:0
        }

        //倍速，当且仅当视频播放时展示
        Rectangle{
            id:playSpeed
            x:playMode.x-width-windowWidth*0.02
            width: 50
            height: parent.height*0.6
            color: "transparent"
            radius: 6
            anchors.verticalCenter: parent.verticalCenter
            visible: mediaType==="video"

            property bool isClicked: false

            Text {
                text: mainWindow.playSpeed==="1.0x"?"倍速":mainWindow.playSpeed
                font.pixelSize: 14
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle{//可选播放模式区域
                parent: myFooter
                anchors.bottom: parent.top
                x:playSpeed.x+playSpeed.width/2-width/2
                width: 60
                color:"transparent"
                visible: playSpeed.isClicked

                ListView{//播放模式列表
                    id:playSpeedList
                    width: parent.width
                    height: 140
                    anchors.bottom: parent.top
                    model:['0.5x','1.0x','1.25x','1.5x','2.0x']
                    delegate: ItemDelegate{//播放模式列表子项
                        id:speedItem
                        width: parent.width
                        height: 28
                        hoverEnabled: true

                        Text {
                            text: qsTr(modelData)
                            font.pixelSize: 14
                            color: modelData===mainWindow.playSpeed?
                                       setColor(255, 100, 41):
                                       (speedItem.hovered?setColor(255, 100, 41):"white")
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        background:Rectangle{
                            color:setColor(0,0,0,0.8)
                        }
                        onClicked: {
                            controller.playSpeed=parseFloat(modelData.substring(0,modelData.length-1),10);
                            mainWindow.playSpeed=modelData;
                            playSpeed.isClicked=false;
                        }
                    }
                }
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: playSpeed.color=setColor(255, 100, 41)
                onExited: playSpeed.color="transparent"
                onClicked: playSpeed.isClicked=!playSpeed.isClicked
            }
        }

        //当前播放模式图标
        Image {
            id: playMode
            source: "qrc:/images/"+playModeList.model[mainWindow.playMode]+".png"
            width: 18
            height: 18
            x:soundSlider.x+windowWidth*0.26
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter

            property bool isClicked: false

            Rectangle{//可选播放模式区域
                parent: myFooter
                anchors.bottom: parent.top
                x:playMode.x+playMode.width/2-width/2
                width: 100
                color:"transparent"
                visible: playMode.isClicked

                ListView{//播放模式列表
                    id:playModeList
                    width: parent.width
                    height: 112
                    anchors.bottom: parent.top
                    model:['随机播放','顺序播放','循环播放','单曲模式']
                    delegate: ItemDelegate{//播放模式列表子项
                        id:modeItem
                        width: parent.width
                        height: 28
                        hoverEnabled: true

                        Text {
                            text: qsTr(modelData)
                            font.pixelSize: 14
                            color: index===mainWindow.playMode?
                                       setColor(255, 100, 41):
                                       (modeItem.hovered?setColor(255, 100, 41):"white")
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        background:Rectangle{
                            color:setColor(0,0,0,0.8)
                        }
                        onClicked: {
                            mainWindow.playMode=index;
                            playMode.isClicked=false;
                        }
                    }
                }
            }

            MouseArea{
                anchors.fill: parent
                onClicked: playMode.isClicked=!playMode.isClicked
            }
        }

        //全屏按钮
        Image {
            id: fullBtn
            source: isFullSreen?"qrc:/images/退出全屏.png":"qrc:/images/全屏.png"
            width: 18
            height: 18
            x:soundSlider.x+windowWidth*0.3
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    isFullSreen=!isFullSreen;
                    isFullSreen?showFull():showInitial();
                }
            }
        }
        
        //播放队列按钮
        Image {
            id: listBtn
            source: "qrc:/images/列表.png"
            width: 18
            height: 18
            x:fullBtn.x+windowWidth*0.04
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(!videoPage.visible){
                        mainWindow.isShowQueue=!mainWindow.isShowQueue;
                        if(mainWindow.isShowQueue){
                            queueAnimation.from = windowWidth;
                            queueAnimation.to = windowWidth-playQueue.width;
                        }else{
                            queueAnimation.from = windowWidth-playQueue.width;
                            queueAnimation.to = windowWidth;
                        }
                    }else{
                        videoPage.isShowQueue=!videoPage.isShowQueue;
                        if(videoPage.isShowQueue){
                            queueAnimation.from = windowWidth;
                            queueAnimation.to = windowWidth-playQueue.width;
                        }else{
                            queueAnimation.from = windowWidth-playQueue.width;
                            queueAnimation.to = windowWidth;
                        }
                    }
                    queueAnimation.target=mainWindow.visible?playQueue:videoQueue;
                    queueAnimation.running = true;
                }
            }
        }
    }
}
