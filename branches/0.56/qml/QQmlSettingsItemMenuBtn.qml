import QtQuick 2.0

Rectangle {
	property alias btnImg: image1.source
	property alias btnTxt: text1.text

	property color hoverColor: "transparent"

	property string componentName: "Rectangle"

	signal trigger(string name)

	function activate() {
		trigger(componentName);
	}

	id: rectangle1
	width: 100
	height: 32
	radius: 5
	color: "transparent"

	Image {
		id: image1
		width: 32
		height: rectangle1.height
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.left: parent.left
		anchors.leftMargin: 5
		smooth: true
		fillMode: Image.PreserveAspectFit
		sourceSize.width: 32
		sourceSize.height: 32
	}

	Text {
		id: text1
		height: rectangle1.height
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.left: image1.right
		anchors.leftMargin: 2
		horizontalAlignment: Text.AlignLeft
		verticalAlignment: Text.AlignVCenter
		font.pixelSize: 12
	}

	MouseArea {
		id: mouseArea1
		height: parent.height
		anchors.fill: parent
		hoverEnabled: true
		onEntered: color = hoverColor;
		onExited: color = "transparent"
		onPressed: {rectangle1.activate();}
	}
}
