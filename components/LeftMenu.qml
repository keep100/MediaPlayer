import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//左侧导航栏
Rectangle{
    id:leftMenu
    width: parent.width/5
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    color: setColor(20, 20, 20,0.8)

    Column{
        width: parent.width

        //logo区域
        Rectangle{
            width: parent.width
            height: leftMenu.height*0.2
            color: "transparent"
            Image {//软件logo
                id: logo
                anchors.centerIn: parent
                source: "qrc:/images/logo.png"
            }
        }

        //分界线
        Rectangle{
            width: parent.width
            height: 1
            color: setColor(183, 184, 184)
        }

        //媒体库标题
        Label{
            text:'媒体库'
            width: parent.width
            height: 50
            font.pixelSize: 22
            leftPadding: 10
            color: setColor(126, 134, 144)
            verticalAlignment: Text.AlignVCenter
        }

        //分界线
        Rectangle{
            id:line
            width: parent.width
            height: 1
            color: setColor(183, 184, 184)
        }

        //导航列表
        ListView{
            id:listView
            width: parent.width
            height: 30*model.length
            model:[
                {
                    text:'视频',
                    icon:'qrc:/images/视频.png'
                },
                {
                    text:'音乐',
                    icon:'qrc:/images/音乐.png'
                },
            ]
            delegate: ItemDelegate{//列表项
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
                onClicked: {
                    listView.currentIndex=index;
                    curIdx=index;
                }
            }
        }
    }
}

