import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Popup {
    id: errorPopup
    anchors.centerIn: parent
    topPadding: 10
    bottomPadding: 10
    leftPadding: 20
    rightPadding: 20
    property string errorInfo: ""    //错误信息

    background: Rectangle{
        anchors.fill: parent
        color:setColor(29, 34, 36)
        radius: 10
    }
    contentItem: Text {
        text: qsTr(errorInfo)
        font.pixelSize: 13
        color: "white"
    }

    Timer{
        running: errorPopup.visible
        interval: 2000
        onTriggered: errorPopup.close()
    }
}

