import QtQuick 2.7

Rectangle {
	id: root
	width: txt.width + 2
	height: txt.height + 2
	color: "transparent"
	radius: 3
	border {
		width: ma.containsMouse ? 1 : 0
		color: borderColor
	}

	property color borderColor: "black"
	property string text: ""
	property string textColor: "black"
	property font textFont: Qt.font({

									})

	signal clicked

	Text {
		id: txt
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter
		text: parent.text
		font: textFont
		color: parent.textColor
	}
	MouseArea {
		id: ma
		anchors.fill: parent
		hoverEnabled: true
		onClicked: root.clicked()
	}
}
