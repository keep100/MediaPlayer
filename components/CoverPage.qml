import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

//音乐封面页面
Image {
    id: coverPage
    source: "../images/cover.png"
    width: windowWidth
    y:windowHeight
    height: windowHeight-footer.height

    //下滑箭头
    Image {
        source: "../images/下箭头.png"
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
