import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

Popup{
    id: configDialog
    modal: true
    dim: true
    anchors.centerIn: parent
    closePolicy: Popup.NoAutoClose

    //重置背景图与模糊度
    function reset(){
        pathInput.text=mainWindow.bgPath==="qrc:/images/bg.jpg"?"":mainWindow.bgPath;
        blurSlider.value=mainWindow.blur;
    }

    background: Rectangle {
        color: setColor(29, 34, 36)
        border.color: "#444"
        border.width: 2
    }
    contentItem: Column {
        anchors.fill: parent
        leftPadding: 15

        Row{
            topPadding: 5
            bottomPadding: 10
            Image {
                source: "qrc:/images/logo.png"
                width: 30
                height: 30
                anchors.verticalCenter: parent.verticalCenter
            }
            Label{
                text: "设置"
                leftPadding: 10
                rightPadding: 320
                font.pixelSize: 13
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }
            Rectangle{
                id:closeBtn
                color: isEnter?setColor(247, 68, 68):"transparent"
                width: 18
                height: 18
                anchors.verticalCenter: parent.verticalCenter
                property bool isEnter: false

                Image {
                    source: "qrc:/images/关闭.png"
                    width: 13
                    height: 13
                    anchors.centerIn: parent
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: closeBtn.isEnter=true
                        onExited: closeBtn.isEnter=false
                        onClicked: {
                            configDialog.close();
                            reset();
                        }
                    }
                }
            }
        }
        //背景图片路径
        Row{
            topPadding: 20
            bottomPadding: 20
            Label{
                text: '背景图片: '
                width: 90
                font.pixelSize: 14
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }
            //背景图片路径框
            TextField{
                id:pathInput
                placeholderText: qsTr("图片路径")
                placeholderTextColor:setColor(136, 138, 146)
                color: "white"
                font.pixelSize: 14
                anchors.verticalCenter: parent.verticalCenter
                selectByMouse:true  //鼠标可以选择文字

                background: Rectangle {
                    implicitWidth: 200
                    color: setColor(30, 45, 56)
                    radius: 5
                }
            }
            Rectangle{
                width: 20
                height: 1
                color: "transparent"
            }
            //选择路径按钮
            Button{
                hoverEnabled:false
                property bool isHover: false

                contentItem: Text {
                    text: "选择路径"
                    font.pixelSize: 12
                    color:  parent.isHover? "white" : setColor(17, 204, 253)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 28
                    color: parent.isHover?setColor(25, 116, 185):setColor(30, 45, 56)
                    radius: 4
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.isHover=true
                    onExited: parent.isHover=false
                    onClicked: fileDialog.open()
                }
            }
        }
        //背景模糊度
        Row{
            topPadding: 20
            bottomPadding: 80
            Label{
                text: '背景模糊度: '
                width: 90
                font.pixelSize: 14
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }
            //模糊度调节条
            Slider {
                id: blurSlider
                from: 0
                value: 0
                to:60
                stepSize: 1
                pressed: false
                anchors.verticalCenter: parent.verticalCenter

                background: Rectangle {//控制条背景
                    y: blurSlider.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width: blurSlider.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "white"

                    Rectangle {//滑块左侧控制条部分背景
                        width: blurSlider.visualPosition * parent.width
                        height: parent.height
                        color: setColor(17, 204, 253)
                        radius: 2
                    }
                }

                handle: Rectangle {//滑块
                    x:  blurSlider.visualPosition * (blurSlider.availableWidth - width)
                    y:  blurSlider.availableHeight / 2 - height / 2
                    implicitWidth: 3
                    implicitHeight: 15
                    color: "white"
                }
            }
        }
        //按钮操作区
        Row{
            leftPadding: 200
            spacing: 20
            rightPadding: 20
            bottomPadding: 10

            Button{
                hoverEnabled:false
                property bool isHover: false

                contentItem: Text {
                    text: "确认"
                    font.pixelSize: 12
                    color:  parent.isHover? "white" : setColor(17, 204, 253)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 28
                    color: parent.isHover?setColor(25, 116, 185):setColor(30, 45, 56)
                    radius: 4
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.isHover=true
                    onExited: parent.isHover=false
                    onClicked: {
                        configDialog.close();
                        mainWindow.bgPath=pathInput.text;
                        mainWindow.blur=blurSlider.value;
                    }
                }
            }

            Button{
                hoverEnabled:false
                property bool isHover: false

                contentItem: Text {
                    text: "取消"
                    font.pixelSize: 12
                    color:  parent.isHover? "white" : setColor(17, 204, 253)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 28
                    color: parent.isHover?setColor(25, 116, 185):setColor(30, 45, 56)
                    radius: 4
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.isHover=true
                    onExited: parent.isHover=false
                    onClicked: {
                        configDialog.close();
                        reset();
                    }
                }
            }
        }
    }

    //本地文件选择框
    FileDialog {
        id: fileDialog
        title:"选择图片"
        nameFilters:["image(*.png *.jpg *.jpeg)"]  //筛选能够选择的文件类型
        onAccepted: pathInput.text=file
    }
}

