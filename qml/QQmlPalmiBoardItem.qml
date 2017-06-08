import QtQuick 2.0

Rectangle {
    id: root
    width: boardTxt.contentWidth
    height: boardTxt.contentHeight

    radius: 4
    border.width: 1
    border.color: colorDark

    color: colorLight

    property string name: ""
    property color colorDark: "black"
    property color colorLight: "white"
    Text {
        id: boardTxt
        font.pixelSize: 10
        text: root.name
    }

    MouseArea {
        anchors.fill: parent
        onClicked: clicked(root.name)
    }

    signal clicked(string name)
}
