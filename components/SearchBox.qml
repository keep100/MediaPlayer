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

    //根据搜索关键词筛选音视频
    function getSearchList(keyword,list){
        return list.filter(item=>item.fileName.toLocaleLowerCase().includes(keyword))
    }

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
            if(curIdx){
                musicView.searchList=getSearchList(text,dataMgr.audioList);
                musicView.isSearched=true;
            }else{
                videoView.searchList=getSearchList(text,dataMgr.videoList);
                videoView.isSearched=true;
            }
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

            onClicked:{//清空关键词，展示列表全部数据
                searchInput.clear();
                searchInput.focus=false;
                curIdx?musicView.isSearched=false:videoView.isSearched=false;
            }
        }
    }
}

