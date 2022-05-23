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

    //获取音频专辑封面
    function getCover(path){
        return path? "file:///"+path : "qrc:/images/唱片.png"
    }
    //获取背景大图
    function getBg(path){
        return path? "file:///"+path : "qrc:/images/coverBg.jpg"
    }

    //音乐专辑封面大图
    Image {
        id:coverImg
        source: getBg(dataMgr?.curAudio.imgPath)
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
            source: getCover(dataMgr?.curAudio.imgPath)
            width: 300
            height: 300
            smooth: true
        }
        //音乐歌手、专辑信息区
        Column{
            leftPadding: 100
            anchors.verticalCenter: parent.verticalCenter
            Text {//歌名
                text: dataMgr?.curAudio.fileName ?? "歌名"
                font.pixelSize: 22
                font.letterSpacing: 1
                color: "white"
                bottomPadding: 20
            }
            Text {//艺人
                text: "艺人: "+dataMgr?.curAudio.artist ?? "艺人"
                font.pixelSize: 13
                font.letterSpacing: 1
                width: 200
                clip: true
                elide: Text.ElideRight
                color: "white"
                bottomPadding: 10
            }
            Text {//专辑
                text: "专辑: "+dataMgr?.curAudio.album ?? "专辑"
                font.pixelSize: 13
                font.letterSpacing: 1
                width: 200
                clip: true
                elide: Text.ElideRight
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


