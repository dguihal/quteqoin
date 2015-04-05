import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
	id:palmipede
	height: 94
	width: 400

	property int lineHeight: 30


	/*************************************************
	 * Post Line input
	 *************************************************/
	Rectangle {
		id: postInput
		anchors {
			left: palmipede.left
			leftMargin: 2
			right: postBtn.left
			rightMargin: 2
			top:palmipede.top
			topMargin: 2
		}
		height: palmipede.lineHeight
		radius: 4

		TextField {
			id: postInputTF
			anchors.fill: parent
			placeholderText: "coin ! coin !"

			style: TextFieldStyle {
				background: Rectangle {
					color: "transparent"
					border.width: 0
				}
			}

			onAccepted: palmipede.doPost()
		}

		property alias text: postInputTF.text
	}

	/*************************************************
	 * Board Selector
	 *************************************************/
	ComboBox {
		id: boardSelector
		anchors {
			right: postBtn.left
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}

		model: ListModel {
			id: boardSelectorItems

			onRowsInserted: boardSelector.doValueChanged()
		}
		textRole: "name"

		onCurrentIndexChanged: doValueChanged()

		function doValueChanged() {
			if(currentIndex >= 0 &&
					currentIndex < boardSelectorItems.count) {
				postInput.color = boardSelectorItems.get(currentIndex).colorLight
				postInput.border.color = boardSelectorItems.get(currentIndex).color
			}
		}
	}

	/*************************************************
	 * Post Button
	 *************************************************/
	Rectangle {
		id: postBtn
		anchors {
			right: attachBtn.left
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}
		border.color: "darkgray"
		color: "transparent"
		height: palmipede.lineHeight
		radius: 4
		width: postBtnTxt.contentWidth + 4

		Text {
			id: postBtnTxt
			anchors.centerIn: parent
			text: qsTr("Post")
		}

		MouseArea {
			anchors.fill: parent
			onPressed: palmipede.doPost()
		}
	}

	/*************************************************
	 * Attach Button
	 *************************************************/
	Rectangle {
		id: attachBtn
		anchors {
			right: palmipede.right
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}
		border.color: "darkgray"
		color: "transparent"
		height: palmipede.lineHeight
		radius: 4
		width: height


		Image {
			id: attachBtnImg
			anchors {
				centerIn: parent
			}
			source: "qrc:/img/attach.png"
		}

		MouseArea {
			anchors.fill: parent
			onPressed: palmipede.doPost()
		}
	}

	/*************************************************
	 * Bold Button
	 *************************************************/
	Button {
		id: boldBtn
		height: 30
		anchors {
			left: palmipede.left
			leftMargin: 2
			top: postInput.bottom
			topMargin: 2
		}

		text: qsTr("Bold")
		style: ButtonStyle {
			label: Text {
				renderType: Text.NativeRendering
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.bold: true
				text: boldBtn.text
			}
		}
	}

	Button {
		id: italicBtn
		anchors {
			left: boldBtn.right
			leftMargin: 2
			top: boldBtn.top
			bottom: boldBtn.bottom
		}

		text: qsTr("Italic")
		style: ButtonStyle {
			label: Text {
				renderType: Text.NativeRendering
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.italic: true
				text: italicBtn.text
			}
		}
	}

	Button {
		id: underlineBtn
		anchors {
			left: italicBtn.right
			leftMargin: 2
			top: italicBtn.top
			bottom: italicBtn.bottom
		}

		text: qsTr("Underline")
		style: ButtonStyle {
			label: Text {
				renderType: Text.NativeRendering
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.underline: true
				text: underlineBtn.text
			}
		}
	}

	Button {
		id: strikeBtn
		anchors {
			left: underlineBtn.right
			leftMargin: 2
			top: underlineBtn.top
			bottom: underlineBtn.bottom
		}

		text: qsTr("Strike")
		style: ButtonStyle {
			label: Text {
				renderType: Text.NativeRendering
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.strikeout: true
				text: strikeBtn.text
			}
		}
	}

	Button {
		id: momentBtn
		height: 30
		anchors {
			left: palmipede.left
			leftMargin: 2
			top: boldBtn.bottom
			topMargin: 2
		}

		text: qsTr("====> Moment < ====")
		style: ButtonStyle {
			label: Text {
				renderType: Text.NativeRendering
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				text: momentBtn.text
			}
		}
	}

	ComboBox {
		id: blamPafSelector
		anchors {
			left: momentBtn.right
			leftMargin: 2
			verticalCenter: momentBtn.verticalCenter
		}

		model: [ "_o/* BLAM!", "_o/* paf!"]

		style: ComboBoxStyle {
			font {
				bold: true
			}
		}
	}


	states: [
		State {
			name: "MAXIMIZED"
			PropertyChanges {
				target: palmipede
				height: 94
			}
			PropertyChanges {
				target: boardSelector
				anchors {
					right: palmipede.right
					rightMargin: 2
					verticalCenter: boldBtn.verticalCenter
				}
			}
			PropertyChanges {
				target: postInput
				anchors {
					right: postBtn.left
					rightMargin: 2
					top:palmipede.top
					topMargin: 2
				}
			}
			PropertyChanges {
				target: boldBtn
				visible: true
			}
			PropertyChanges {
				target: italicBtn
				visible: true
			}
			PropertyChanges {
				target: underlineBtn
				visible: true
			}
			PropertyChanges {
				target: strikeBtn
				visible: true
			}
			PropertyChanges {
				target: momentBtn
				visible: true
			}
			PropertyChanges {
				target: blamPafSelector
				visible: true
			}
		},
		State {
			name: "MINIMIZED"
			PropertyChanges {
				target: palmipede
				height: 30
			}
			PropertyChanges {
				target: boardSelector
				anchors {
					right: postBtn.left
					rightMargin: 2
					verticalCenter: postInput.verticalCenter
				}
			}
			PropertyChanges {
				target: postInput
				anchors {
					right: boardSelector.left
					rightMargin: 2
				}
			}
			PropertyChanges {
				target: boldBtn
				visible: false
			}
			PropertyChanges {
				target: italicBtn
				visible: false
			}
			PropertyChanges {
				target: underlineBtn
				visible: false
			}
			PropertyChanges {
				target: strikeBtn
				visible: false
			}
			PropertyChanges {
				target: momentBtn
				visible: false
			}
			PropertyChanges {
				target: blamPafSelector
				visible: false
			}
		}
	]

	signal post (string bouchot, string message)

	function addBouchot(name, c, cL) {
		boardSelectorItems.append({ "name": name,
									  "color": "" + c,
									  "colorLight": "" + cL })
	}

	function doPost () {
		post(boardSelector.currentText, postInput.text)
		postInput.text = ""
	}

	signal insertText(string txt)
	onInsertText: {
	}

	signal insertReplaceText(string head, string tail)
	onInsertReplaceText: {

	}
}
