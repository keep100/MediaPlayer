import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

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
    contentItem: ScrollView{
        id:scrollview
        ScrollBar.vertical: ScrollBar {
            parent: scrollview
            id:control
            x: scrollview.mirrored ? 0 : scrollview.width - width
            y: scrollview.topPadding
            height: 0
            visible: true

            background:Rectangle{
                color: setColor(29, 34, 36)
            }

            contentItem: Rectangle {
                implicitWidth: 6
                visible: false
            }
        }
        Column {
            anchors.fill: parent
            leftPadding: 15
            //信息区域分割线
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
            //弹窗标题栏
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
            //文件名称
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
                                            width: 170
                    clip: true
                    wrapMode: Text.WrapAnywhere
                    lineHeightMode:Text.FixedHeight
                    lineHeight: 22
                    font.pixelSize: 13
                    color: "white"
                }
            }

            Loader{sourceComponent: divider}
            //文件类型
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
                    text: file?.filePath.substring(file?.filePath.lastIndexOf('.')+1) ?? "-"
                    font.pixelSize: 13
                    color: "white"
                }
            }
            //文件位置
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
                    width: 180
                    clip: true
                    wrapMode: Text.WrapAnywhere
                    lineHeightMode:Text.FixedHeight
                    lineHeight: 22
                    font.pixelSize: 13
                    color: "white"
                }
            }
            //文件时长
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: '时长:'
                    width: 110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: formatTime(file?.duration)
                    font.pixelSize: 13
                    color: "white"
                }
            }

            Loader{sourceComponent: divider}
            //音频专辑信息或视频帧宽度信息
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: mediaType==='音频'?'专辑:':'帧宽度:'
                    width: 110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: file?.isAudio? (file?.album? file?.album : "-"):(file?.width? file?.width:"-")
                    width: 170
                    clip: true
                    wrapMode: Text.WrapAnywhere
                    lineHeightMode:Text.FixedHeight
                    lineHeight: 22
                    font.pixelSize: 13
                    color: "white"
                }
            }
            //音频歌手信息或视频帧高度信息
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: mediaType==='音频'?'歌手: ':'帧高度: '
                    width:110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: file?.isAudio? (file?.artist? file?.artist:"-"):(file?.height? file?.height:"-")
                    width: 170
                    clip: true
                    wrapMode: Text.WrapAnywhere
                    lineHeightMode:Text.FixedHeight
                    lineHeight: 22
                    font.pixelSize: 13
                    color: "white"
                }
            }

            Loader{sourceComponent: divider}

            //视频比特率
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: '视频比特率: '
                    width:110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: file?.vBitRate? file?.vBitRate+" bps" : "-"
                    font.pixelSize: 13
                    color: "white"
                }
            }
            //音频比特率
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: '音频比特率: '
                    width:110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: file?.aBitRate? file?.aBitRate+" bps" : "-"
                    font.pixelSize: 13
                    color: "white"
                }
            }
            //声道
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: '声道: '
                    width:110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: file?.channels? (file?.channels===2?file?.channels+" (立体音)":file?.channels) : "-"
                    font.pixelSize: 13
                    color: "white"
                }
            }
            //编码格式
            Row{
                topPadding: 10
                bottomPadding: 10
                Label{
                    text: '编码格式: '
                    width:110
                    font.pixelSize: 13
                    color: "white"
                }
                Label{
                    text: file?.codecId? file?.codecId : "-"
                    font.pixelSize: 13
                    color: "white"
                }
            }
        }
    }
}

