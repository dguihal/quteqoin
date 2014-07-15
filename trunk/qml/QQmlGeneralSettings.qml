import QtQuick 2.0

QQmlSettingsItem {
	SystemPalette { id: activePalette }

	id: rectangle1
	width: 300
	height: 400
	color: activePalette.window

	Grid {
		id: grid1
		height: 133
		anchors.right: parent.right
		anchors.rightMargin: 2
		anchors.left: parent.left
		anchors.leftMargin: 2
		anchors.top: parent.top
		anchors.topMargin: 2

		columns: 2
		spacing: 3

		Text {
			id: text1
			text: qsTr("Max history size :")
			font.pixelSize: 12
		}

		Rectangle {
			id: rectangleTextInput1
			width: 150
			height: text1.height
			color: activePalette.base
			radius: 4

			TextInput {
				id: textInput1
				text: qsTr("")
				anchors.fill: parent
				font.pixelSize: 12
			}
		}

		Text {
			id: text2
			text: qsTr("Default User-Agent :")
			font.pixelSize: 12
		}

		Rectangle {
			id: rectangleTextInput2
			width: 150
			height: text2.height
			color: activePalette.base
			radius: 4

			TextInput {
				id: textInput2
				text: qsTr("")
				anchors.fill: parent
				font.pixelSize: 12
			}
		}

		Text {
			id: text3
			text: qsTr("Default login :")
			font.pixelSize: 12
		}

		Rectangle {
			id: rectangleTextInput3
			width: 150
			height: text2.height
			color: activePalette.base
			radius: 4

			TextInput {
				id: textInput3
				text: qsTr("")
				font.pixelSize: 12
			}
		}
	}
}

