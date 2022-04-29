import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import "./components/"

Window {
    id:mainWindow
    width: windowWidth
    height: windowHeight
    minimumWidth: 800
    minimumHeight: 500
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinimizeButtonHint //除去窗口原生标题栏

    property int windowWidth: Screen.desktopAvailableWidth*0.65 //窗口宽度，跟随电脑屏幕变化
    property int windowHeight: Screen.desktopAvailableHeight*0.8 ////窗口高度，跟随电脑屏幕变化
    //    property int curIdx: 0

    //设置color
    function setColor(r,g,b,a=1){
        return Qt.rgba(r/255,g/255,b/255,a)
    }
    //实现窗口最大化
    function showMax(){
        showMaximized();
        windowWidth=Screen.desktopAvailableWidth;
        windowHeight=Screen.desktopAvailableHeight;
    }
    //实现窗口正常化
    function showInitial(){
        showNormal();
        windowWidth=Screen.desktopAvailableWidth*0.65;
        windowHeight=Screen.desktopAvailableHeight*0.8;
    }
    //实现窗口全屏
    function showFull(){
        showFullScreen();
        windowWidth=Screen.width;
        windowHeight=Screen.height;
    }

    Image {
        id: bg
        source: "./images/bg.jpg"
        width: windowWidth
        height: windowHeight
        smooth: true
    }

    Rectangle{
        id:leftMenu
        width: parent.width/5
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: setColor(20, 20, 20,0.8)

        Column{
            Rectangle{
                width: leftMenu.width
                height: leftMenu.height*0.2
                color: "transparent"
                Image {
                    id: logo
                    anchors.centerIn: parent
                    source: "./images/logo.png"
                }
            }
            Rectangle{
                width: leftMenu.width
                height: 1
                color: setColor(183, 184, 184)
            }
            Label{
                text:'媒体库'
                width: leftMenu.width
                height: 50
                font.pixelSize: 22
                leftPadding: 10
                color: setColor(126, 134, 144)
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle{
                id:line
                width: leftMenu.width
                height: 1
                color: setColor(183, 184, 184)
            }
            ListView{
                id:listView
                width: leftMenu.width
                height: 30*model.length
                model:[
                    {
                        text:'视频',
                        icon:'./images/视频.png'
                    },
                    {
                        text:'音乐',
                        icon:'./images/音乐.png'
                    },
                ]

                delegate: ItemDelegate{
                    width: parent.width
                    height: 30

                    Text{
                        text:modelData.text
                        font.pixelSize: 14
                        leftPadding:30
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    background: Rectangle{
                        Image {
                            x:5
                            width: 18
                            height: 18
                            anchors.verticalCenter: parent.verticalCenter
                            fillMode: Image.PreserveAspectFit
                            source: modelData.icon
                        }
                        color: listView.currentIndex===index?setColor(70, 117, 156,0.8):"transparent"
                    }
                    onClicked: listView.currentIndex=index
                }
            }
        }
    }

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

            Label{
                text: listView.currentIndex===0?'视频':'音乐'
                font.pixelSize: 24
                color: "white"
            }

            Rectangle{
                id:searchBox
                x:parent.width-width-40
                width: 240
                height: 30
                color: setColor(54, 56, 61)
                radius: 20

                Image {
                    id:searchIcon
                    width: 22
                    height: 22
                    source: "./images/搜索.png"
                    x:12
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextField{
                    id:searchInput
                    placeholderText: qsTr("搜索")
                    placeholderTextColor:setColor(136, 138, 146)
                    color: "white"
                    font.pixelSize: 14
                    x:searchIcon.x+searchIcon.width+8
                    anchors.verticalCenter: parent.verticalCenter
                    selectByMouse:true

                    background: Rectangle {
                        implicitWidth: 170
                        color: "transparent"
                    }

                    onAccepted: {
                        console.log(text)
                        focus=false;
                    }
                }

                Image {
                    id:cancelBtn
                    width: 18
                    height: 18
                    source: "./images/取消搜索词.png"
                    x:parent.width-width-8
                    anchors.verticalCenter: parent.verticalCenter

                    MouseArea{
                        anchors.fill: parent

                        onClicked: searchInput.clear()
                    }
                }
            }


        }

        Button{
            id: uploadBtn
            x: titleRow.x+30
            y:titleRow.y+titleRow.height+8
            text: listView.currentIndex===0?"导入视频":"导入音频"
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
        }

        ListData {id:videoList }

        Rectangle {
            y:uploadBtn.y+uploadBtn.height+20
            width: parent.width
            height: parent.height-y
            color: "transparent"

            Component {
                id: videoDelegate
                Item {
                    width: grid.cellWidth
                    height: grid.cellHeight
                    Column {
                        anchors.fill: parent
                        anchors.leftMargin: titleBar.isMaximized||footer.isFullSreen?50:30
                        anchors.bottomMargin: titleBar.isMaximized||footer.isFullSreen?25:15
                        Image {
                            width: parent.width
                            height: parent.height*0.8
                            source: src
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            text: videoName
                            font.pixelSize:titleBar.isMaximized||footer.isFullSreen?16:14
                            topPadding: 8
                            color: "white"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }

            GridView {
                id: grid
                anchors.fill: parent
                cellWidth:  titleBar.isMaximized||footer.isFullSreen?240:180
                cellHeight: cellWidth*0.7
                clip: true

                model:videoList
                delegate: videoDelegate
            }
        }
    }

    //标题栏
    TitleBar{id:titleBar}

    Rectangle{
        id:footer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 40
        color: setColor(0,0,0,0.8)

        property bool isFullSreen: false

        Row{
            width: parent.width
            height: parent.height
            Image {
                id: exitBtn
                source: "./images/停止.png"
                width: 20
                height: 20
                x:leftMenu.width+windowWidth*0.1
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }
            Image {
                id: prevBtn
                source: "./images/上一首.png"
                width: 26
                height: 26
                x:exitBtn.x+windowWidth*0.08
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }
            Image {
                id: playBtn
                source: "./images/播放.png"
                width: 15
                height: 15
                x:prevBtn.x+windowWidth*0.07
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }
            Image {
                id: nextBtn
                source: "./images/下一首.png"
                width: 26
                height: 26
                x:playBtn.x+windowWidth*0.06
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }
            Image {
                id: soundIcon
                source: "./images/音量.png"
                width: 16
                height: 16
                x:nextBtn.x+windowWidth*0.08
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                id: soundSlider
                value: 0.5
                x:soundIcon.x+windowWidth*0.02
                anchors.verticalCenter: parent.verticalCenter

                background: Rectangle {
                    y: soundSlider.availableHeight / 2 - height / 2
                    implicitWidth: 100
                    implicitHeight: 4
                    width: soundSlider.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "white"

                    Rectangle {
                        width: soundSlider.visualPosition * parent.width
                        height: parent.height
                        color: setColor(22, 132, 252)
                        radius: 2
                    }
                }

                handle: Rectangle {
                    x:  soundSlider.visualPosition * (soundSlider.availableWidth - width)
                    y:  soundSlider.availableHeight / 2 - height / 2
                    implicitWidth: 13
                    implicitHeight: 13
                    radius: 100
                    color: soundSlider.pressed ? "#f0f0f0" : "#f6f6f6"
                    border.color: "#bdbebf"
                }
            }
            Image {
                id: fullBtn
                source: footer.isFullSreen?"./images/退出全屏.png":"./images/全屏.png"
                width: 18
                height: 18
                x:soundSlider.x+windowWidth*0.3
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        footer.isFullSreen=!footer.isFullSreen;
                        footer.isFullSreen?showFull():showInitial();
                    }
                }
            }
            Image {
                id: listBtn
                source: "./images/列表.png"
                width: 18
                height: 18
                x:fullBtn.x+windowWidth*0.04
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}