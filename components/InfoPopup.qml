import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Popup{
    id: morePopup
    modal: true
    dim: true
    anchors.centerIn: parent
    closePolicy: Popup.NoAutoClose
    property var file: null       //文件对象
    required property string mediaType

    background: Rectangle {
        implicitWidth: 270
        color: setColor(29, 34, 36)
        border.color: "#444"
        border.width: 2
    }
    contentItem: Column {
        anchors.fill: parent
        leftPadding: 15
        Component{
            id:divider
            Row{
                topPadding: 10
                bottomPadding: 10
                Rectangle{
                    width: 300
                    height: 1
                    color: "gray"
                }
            }
        }

        Row{
            topPadding: 5
            bottomPadding: 5
            Image {
                source: "qrc:/images/logo.png"
                width: 30
                height: 30
                anchors.verticalCenter: parent.verticalCenter
            }
            Label{
                text: mediaType==='音频'?'音频信息':'视频信息'
                leftPadding: 10
                rightPadding: 190
                font.pixelSize: 13
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }
            Rectangle{
                id:popUpCloseBtn
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
                        onEntered: popUpCloseBtn.isEnter=true
                        onExited: popUpCloseBtn.isEnter=false
                        onClicked: morePopup.close()
                    }
                }
            }
        }

        Row{
            topPadding: 10
            bottomPadding: 10
            Label{
                text: mediaType==='音频'?'音频名称:':'视频名称:'
                width: 110
                font.pixelSize: 13
                color: "white"
            }
            Label{
                text: file?.fileName ?? "-"
                font.pixelSize: 13
                color: "white"
            }
        }

        Loader{sourceComponent: divider}

        Row{
            topPadding: 10
            bottomPadding: 10
            Label{
                text: '文件类型:'
                width: 110
                font.pixelSize: 13
                color: "white"
            }
            Label{
                text: file?.filePath.split('.')[1] ?? "-"
                font.pixelSize: 13
                color: "white"
            }
        }

        Row{
            topPadding: 10
            bottomPadding: 10
            Label{
                text: '目标位置:'
                width: 110
                font.pixelSize: 13
                color: "white"
            }
            Label{
                text: file?.filePath.substring(0,file?.filePath.lastIndexOf('/')) ?? "-"
                font.pixelSize: 13
                color: "white"
            }
        }

        Row{
            topPadding: 10
            bottomPadding: 10
            Label{
                text: '文件大小:'
                width: 110
                font.pixelSize: 13
                color: "white"
            }
            Label{
                text:"536 MB"
                font.pixelSize: 13
                color: "white"
            }
        }

        Loader{sourceComponent: divider}

        Row{
            topPadding: 10
            bottomPadding: 10
            Label{
                text: mediaType==='音频'?'专辑:':'视频帧率:'
                width: 110
                font.pixelSize: 13
                color: "white"
            }
            Label{
                text: file?.isAudio? (file?.album? file?.album : "-"):"-"
                width: 170
                clip: true
                wrapMode: Text.WrapAnywhere
                lineHeightMode:Text.FixedHeight
                lineHeight: 22
                font.pixelSize: 13
                color: "white"
            }
        }

        Row{
            topPadding: 10
            bottomPadding: 30
            Label{
                text: mediaType==='音频'?'歌手: ':'分辨率: '
                width:110
                font.pixelSize: 13
                color: "white"
            }
            Label{
                text: file?.isAudio? (file?.artist? file?.artist:"-"):"-"
                width: 170
                clip: true
                wrapMode: Text.WrapAnywhere
                lineHeightMode:Text.FixedHeight
                lineHeight: 22
                font.pixelSize: 13
                color: "white"
            }
        }
    }
}

