import QtQuick 2.7

Rectangle {
	id: root
	width: boardTxt.contentWidth + 4
	height: boardTxt.contentHeight + 2

	radius: 4
	border.width: 1
	border.color: colorDark

	color: colorLight

	property string name: ""
	property color colorDark: "black"
	property color colorLight: "white"

	Text {
		id: boardTxt

		anchors.centerIn: root

		font.pixelSize: 10
		text: root.name
	}

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true

		onClicked: root.clicked(root.name)
		onEntered: root.border.color = "black"
		onExited: root.border.color = root.colorDark
	}

	signal clicked(string name)
}
