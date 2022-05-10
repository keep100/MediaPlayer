import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//搜索框
Rectangle{
    id:searchBox
    x:parent.width-width-40
    width: 240
    height: 30
    color: setColor(54, 56, 61)
    radius: 20

    //搜索图标
    Image {
        id:searchIcon
        width: 22
        height: 22
        source: "qrc:/images/搜索.png"
        x:12
        anchors.verticalCenter: parent.verticalCenter
    }

    //搜索内容输入框
    TextField{
        id:searchInput
        placeholderText: qsTr("搜索")
        placeholderTextColor:setColor(136, 138, 146)
        color: "white"
        font.pixelSize: 14
        x:searchIcon.x+searchIcon.width+8
        anchors.verticalCenter: parent.verticalCenter
        selectByMouse:true  //鼠标可以选择文字

        background: Rectangle {
            implicitWidth: 170
            color: "transparent"
        }

        onAccepted: {//按下enter回车键触发，进行搜索
            console.log(text)
            focus=false;
        }
    }

    //清除搜索关键词图标
    Image {
        id:cancelBtn
        width: 18
        height: 18
        source: "qrc:/images/取消搜索词.png"
        x:parent.width-width-8
        anchors.verticalCenter: parent.verticalCenter

        MouseArea{
            anchors.fill: parent

            onClicked: searchInput.clear()
        }
    }
}

