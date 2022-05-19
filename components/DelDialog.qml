import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Dialog {
    id: delDialog
    modal: true
    anchors.centerIn: parent
    closePolicy: Popup.NoAutoClose
    required property string mediaType

    background: Rectangle{
        anchors.fill: parent
        color:setColor(29, 34, 36)
    }

    header: Row{//对话框头部
        leftPadding: 15
        topPadding: 15
        bottomPadding: 10
        rightPadding: 10
        Label{
            text: '确定要从该媒体库中移除该'+mediaType+'?'
            font.pixelSize: 15
            rightPadding: 40
            color: "white"
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
                    onClicked: delDialog.close()
                }
            }
        }
    }

    footer: Row{//对话框底部按钮区域
        layoutDirection:Qt.RightToLeft
        spacing: 15
        rightPadding: 10
        bottomPadding: 10

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
                onClicked: delDialog.close()
            }
        }

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
                    delDialog.close()

                }
            }
        }
    }
}

