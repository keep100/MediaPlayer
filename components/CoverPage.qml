import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import Qt5Compat.GraphicalEffects

//音乐封面页面
Item{
    id: coverPage
    width: windowWidth
    y:windowHeight
    height: windowHeight-footer.height

    //音乐专辑封面大图
    Image {
        id:coverImg
        source: "file:///D:/Qtcode/build-MediaPlayer-Desktop_Qt_6_3_0_MSVC2019_64bit-Debug/Data/thumpnail/audio/0ac357471139b5f32b496d4d7e2b72b2.jpg"
        width: parent.width
        height: parent.height
        smooth: true
    }

    //毛玻璃效果
    FastBlur {
        anchors.fill: coverImg
        source: coverImg
        radius: 45
    }

    //音乐信息展示区
    Row{
        anchors.centerIn: parent
        //音乐略缩图
        Image {
            source: "file:///D:/Qtcode/build-MediaPlayer-Desktop_Qt_6_3_0_MSVC2019_64bit-Debug/Data/thumpnail/audio/0ac357471139b5f32b496d4d7e2b72b2.jpg"
            width: 300
            height: 300
            smooth: true
        }
        //音乐歌手、专辑信息区
        Column{
            leftPadding: 100
            anchors.verticalCenter: parent.verticalCenter
            Text {//歌名
                text: qsTr("记忆停留")
                font.pixelSize: 22
                font.letterSpacing: 1
                color: "white"
                bottomPadding: 20
            }
            Text {//艺人
                text: qsTr("艺人:  蓝心羽")
                font.pixelSize: 13
                font.letterSpacing: 1
                color: "white"
                bottomPadding: 10
            }
            Text {//专辑
                text: qsTr("专辑:  记忆停留")
                font.pixelSize: 13
                font.letterSpacing: 1
                color: "white"
            }
        }
    }

    //下滑箭头
    Image {
        source: "qrc:/images/下箭头.png"
        width: 30
        height: 30
        x:20
        y:20
        MouseArea{
            anchors.fill: parent
            onClicked: {
                isCoverShow=false;
                coverAnimation.from=0;
                coverAnimation.to=windowHeight;
                coverAnimation.running=true;
            }
        }
    }
}


