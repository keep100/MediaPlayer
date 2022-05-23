import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1
import Qt5Compat.GraphicalEffects
import "./components/"

Window {
    id:mainWindow
    width: windowWidth
    height: windowHeight
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint  //除去窗口原生标题栏

    property int windowWidth: Screen.desktopAvailableWidth*0.65    //窗口宽度，跟随电脑屏幕变化
    property int windowHeight: Screen.desktopAvailableHeight*0.85  //窗口高度，跟随电脑屏幕变化
    property bool isFullSreen: false      //是否已经全屏
    property bool isAudioPlay: false      //是否有音频播放
    property bool isVideoPlay: false      //是否有视频播放
    property bool isPlaying: false        //音视频是否正在播放
    property bool isCoverShow: false      //音频封面页是否已经展示
    property bool isShowQueue: false      //是否展示了播放列表
    property int playMode: 2              //播放模式，默认循环播放
    property int voice: 15                //播放音量
    property int curIdx: 0                //当前页面，0代表视频页面，1代表音频页面
    property int curMediaIdx: -1          //当前正在播放文件的索引标记
    property string playSpeed: '1.0x'     //当前视频播放速度
    property int blur: 0                  //播放器背景模糊度
    property string bgPath: "qrc:/images/bg.jpg"            //背景图片路径

    //设置color
    function setColor(r,g,b,a=1){
        return Qt.rgba(r/255,g/255,b/255,a)
    }
    //实现窗口最大化
    function showMax(){
        showMaximized();
        windowWidth=Screen.desktopAvailableWidth;
        windowHeight=Screen.desktopAvailableHeight;
        if(!isCoverShow){
            coverPage.y=windowHeight;
        }
    }
    //实现窗口正常化
    function showInitial(){
        showNormal();
        if(isCoverShow){
            coverPage.y=0;
        }
        windowWidth=Screen.desktopAvailableWidth*0.65;
        windowHeight=Screen.desktopAvailableHeight*0.8;
        if(!isCoverShow){
            coverPage.y=windowHeight;
        }
        if(videoPage.visible){
            mainWindow.visible=false;
            videoPage.x=Screen.width*0.5-videoPage.width*0.5;
            videoPage.y=Screen.height*0.5-videoPage.height*0.5;
            videoPage.isEnabled=false;
        }
    }
    //实现窗口全屏
    function showFull(){
        showFullScreen();
        if(isCoverShow){
            coverPage.y=0;
        }
        windowWidth=Screen.width;
        windowHeight=Screen.height;
        if(!isCoverShow){
            coverPage.y=windowHeight;
        }
        if(videoPage.visible){
            mainWindow.visible=false;
            videoPage.x=0;
            videoPage.y=0;
        }
    }
    //格式化时间戳
    function formatTime(time){
        if(typeof time !=='number' ||time<0){
            return;
        }
        let hour=Math.floor(time/3600000);
        time=time%3600000;
        let minute=Math.floor(time/60000);
        time=time%60000;
        let second=Math.floor(time/1000);

        return hour?
                    ([hour,minute,second].map(function(num){
                        num=num.toString();
                        return num[1]?num:'0'+num;
                    })).join(':'):
                    ([minute,second].map(function(num){
                        num=num.toString();
                        return num[1]?num:'0'+num;
                    })).join(':')
    }

    //监听是否有音频在播放
    onIsAudioPlayChanged: {
        if(!isAudioPlay){
            musicView.setIdx(-1);
            playQueue.setIdx(-1);
            controller.exit();
        }
    }

    //监听是否有视频在播放
    onIsVideoPlayChanged: {
        if(!isVideoPlay){
            playQueue.setIdx(-1);
            controller.exit();
        }
    }

    //监听是否正在播放音视频
    onIsPlayingChanged: controller.stop()

    //监听播放模式改变
    onPlayModeChanged: controller.mode=playMode

    //监听音量改变
    onVoiceChanged: controller.voice=voice

    //监听当前播放文件变化
    onCurMediaIdxChanged: {
        playQueue.setIdx(curMediaIdx);
        curIdx?musicView.setIdx(curMediaIdx):videoPage.setIdx(curMediaIdx);
    }

    //监听controller信号
    Connections{
        target: controller
        function onFileImportFail(path){     //文件导入失败
            errorPopup.close();
            errorPopup.errorInfo=
                    path.substring(path.lastIndexOf('/')+1,path.lastIndexOf('.'))+" 导入失败";
            errorPopup.open();
        }
        function onFileMiss(file){           //文件缺失
            delDialog.fileMiss=true;
            delDialog.delIdx=file.index;
            delDialog.mediaType=file.isAudio?'音频':'视频';
            delDialog.open();
        }
        function onFileError(file){          //文件解析失败或者md5不一致
            console.log(file);

        }
        function onFileFinish(){             //文件播放结束
            console.log('fileFinish');
        }
        function onPlayMedia(){              //准备播放视频
            console.log('begin play');
        }
        function onUpdate(yuv){              //渲染组件进行更新
            console.log('update');
        }
    }

    //背景设置对话框
    ConfigDialog{id:configDialog}

    //文件缺失提示框
    DelDialog{id:delDialog}

    //错误信息弹窗
    ErrorPopup{id:errorPopup}

    //视频播放界面
    VideoPage{id:videoPage}

    //窗口背景
    Image {
        id: bg
        source: bgPath
        width: windowWidth
        height: windowHeight
        smooth: true
    }

    //毛玻璃效果
    FastBlur {
        anchors.fill: bg
        source: bg
        radius: blur
    }

    //左侧导航栏
    LeftMenu{id:leftMenu}

    //内容展示区
    Rectangle{
        id:contentView
        anchors.top: titleBar.top
        anchors.left: leftMenu.right
        anchors.right: parent.right
        anchors.bottom: footer.top
        color: setColor(25, 25, 25,0.6)

        Row{
            id:titleRow
            anchors.left: parent.left
            leftPadding: 30
            topPadding: 50
            width: parent.width

            //当前页面标题
            Label{
                text: curIdx===0?'视频':'音乐'
                font.pixelSize: 24
                color: "white"
            }

            //搜索框
            SearchBox{id:searchBox}
        }

        //上传文件按钮
        Button{
            id: uploadBtn
            x: titleRow.x+30
            y:titleRow.y+titleRow.height+8
            text: curIdx===0?"导入视频":"导入音频"
            hoverEnabled:false

            contentItem: Text {
                text: uploadBtn.text
                font.pixelSize: 12
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                implicitWidth: 80
                implicitHeight: 25
                color: uploadBtn.down?setColor(97, 173, 253):setColor(77, 153, 252)
                radius: 20
            }

            onClicked: fileDialog.open()
        }

        //视频展示区
        VideoView{id:videoView}

        //音频展示区
        MusicView{id:musicView}
    }

    //本地文件选择框
    FileDialog {
        id: fileDialog
        fileMode:FileDialog.OpenFiles  //支持文件多选
        title:curIdx===0?"选择视频":"选择音频"
        nameFilters: curIdx===0?["video files (*.mp4 *.flv *.avi *.dat *.mkv)"]  //筛选能够选择的文件类型
                               :["audio files (*.mp3 *.flac *.wav *rm)"]
        onAccepted:{
            const urlList=[];
            for(let url of files){
                urlList.push(url.toString().split('file:///')[1]);
            }
            controller.importData(urlList,curIdx===1)
        }
    }

    //标题栏
    TitleBar{id:titleBar}

    //音乐封面页面
    CoverPage{id:coverPage}

    //音频封面滑动动画
    PropertyAnimation{
        id:coverAnimation
        target: coverPage
        properties: "y"
        duration: 300
        easing.type: Easing.InQuad
    }

    //播放队列
    PlayQueue{
        id:playQueue
        x:isShowQueue?windowWidth-playQueue.width:windowWidth
    }

    //播放队列滑动动画
    PropertyAnimation{
        id: queueAnimation
        properties: "x"
        duration: 500
        easing.type: Easing.InOutExpo //渐变滑出效果
    }

    //音频播放进度条
    Slider {
        id: audioSlider
        from: 0
        value: controller?.time ?? 0
        to: dataMgr?.curAudio.duration ?? 100
        stepSize: 1
        y:footer.y-audioSlider.availableHeight / 2
        z:1
        visible: isAudioPlay

        background: Rectangle {
            y: audioSlider.availableHeight / 2 - height / 2
            implicitWidth: windowWidth
            implicitHeight: 4
            width: audioSlider.availableWidth
            height: implicitHeight
            radius: 2
            color: "white"

            Rectangle {
                width: audioSlider.visualPosition * parent.width
                height: parent.height
                color: setColor(22, 132, 252)
                radius: 2

                //监听鼠标是否进入，对控制点进行显示或隐藏
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        audioSlider.background.implicitHeight=6;
                        audioSlider.handle.visible=true;
                    }

                    onExited: {
                        audioSlider.background.implicitHeight=4;
                        audioSlider.handle.visible=false;
                    }
                }
            }
        }

        handle: Rectangle {
            x:  audioSlider.visualPosition * (audioSlider.availableWidth - width)
            y:  audioSlider.availableHeight / 2 - height / 2
            implicitWidth: 10
            implicitHeight: 10
            radius: 100
            color: audioSlider.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
            visible: false
        }
        onPressedChanged: {//监听最后释放位置
            if(!pressed){
                controller.setTime(value);
            }
        }
    }

    //音频底部控制栏
    Footer{
        id:footer
        mediaType: 'audio'
    }
}
