import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

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
	Rectangle {
		id: boardSelector
		anchors {
			bottom: parent.bottom
			bottomMargin: 2
			right: postBtn.left
			rightMargin: 2
			top: attachBtn.bottom
			topMargin: 2
		}

		Component {
			id: boardSelectorDelegate
			Rectangle {
				width: boardSelectorGV.cellWidth
				height: boardSelectorGV.cellHeight
				color: colorLight
				border {
					color: color
					width: 1
				}
				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					onEntered: {
						console.log(name)
					}
				}
			}
		}

		GridView {
			id: boardSelectorGV
			anchors.fill: parent
			cellWidth: width/(boardSelectorItems.count / 2)
			cellHeight: palmipede.height / 2

			model: ListModel {
				id: boardSelectorItems

				onRowsInserted: boardSelectorGV.doValueChanged()
			}
			delegate: boardSelectorDelegate
			highlight: Rectangle { color: "darkgrey"; radius: 4 }
			focus: true

			function doValueChanged() {
				if(currentIndex >= 0 &&
						currentIndex < boardSelectorItems.count) {
					postInput.color = boardSelectorItems.get(currentIndex).colorLight
					postInput.border.color = boardSelectorItems.get(currentIndex).color
				}
			}
		}
	}
/*
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
	*/

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
	Rectangle {
		id: boldBtn
		height: palmipede.lineHeight
		anchors {
			left: palmipede.left
			leftMargin: 2
			top: postInput.bottom
			topMargin: 2
		}
		border.color: "darkgray"
		color: "transparent"
		radius: 4
		width: Math.max(height, boldBtnTxt.contentWidth + 4)

		Text {
			id: boldBtnTxt

			anchors.centerIn: parent
			text: qsTr("B")
			font {
				family: "Times New Roman"
				bold: true
				pointSize: 16
			}
		}

		MouseArea {
			id: boldBtnMA

			anchors.fill: parent
			onPressed: palmipede.insertSurroundText("<b>", "</b>")
		}
	}

	/*************************************************
	 * Italic Button
	 *************************************************/
	Rectangle {
		id: italicBtn
		anchors {
			left: boldBtn.right
			leftMargin: 2
			top: boldBtn.top
			bottom: boldBtn.bottom
		}
		border.color: "darkgray"
		color: "transparent"
		height: palmipede.lineHeight
		radius: 4
		width: Math.max(height, italicBtnTxt.contentWidth + 4)

		Text {
			id: italicBtnTxt

			anchors.centerIn: parent
			text: qsTr("I")
			font {
				family: "Times New Roman"
				italic: true
				pointSize: 16
			}
		}

		MouseArea {
			id: italicBtnMA

			anchors.fill: parent
			onPressed: palmipede.insertSurroundText("<i>", "</i>")
		}
	}

	/*************************************************
	 * Underline Button
	 *************************************************/
	Rectangle {
		id: underlineBtn
		anchors {
			left: italicBtn.right
			leftMargin: 2
			top: italicBtn.top
			bottom: italicBtn.bottom
		}
		border.color: "darkgray"
		color: "transparent"
		height: palmipede.lineHeight
		radius: 4
		width: Math.max(height, underlineBtnTxt.contentWidth + 4)

		Text {
			id: underlineBtnTxt

			anchors.centerIn: parent
			text: qsTr("U")
			font {
				family: "Times New Roman"
				underline: true
				pointSize: 16
			}
		}

		MouseArea {
			id: underlineBtnMA

			anchors.fill: parent
			onPressed: palmipede.insertSurroundText("<u>", "</u>")
		}
	}

	/*************************************************
	 * Underline Button
	 *************************************************/
	Rectangle {
		id: strikeBtn
		anchors {
			left: underlineBtn.right
			leftMargin: 2
			top: underlineBtn.top
			bottom: underlineBtn.bottom
		}
		border.color: "darkgray"
		color: "transparent"
		height: palmipede.lineHeight
		radius: 4
		width: Math.max(height, strikeBtnTxt.contentWidth + 4)

		Text {
			id: strikeBtnTxt

			anchors.centerIn: parent
			text: qsTr("S")
			font {
				family: "Times New Roman"
				strikeout: true
				pointSize: 16
			}
		}

		MouseArea {
			id: strikeBtnMA

			anchors.fill: parent
			onPressed: palmipede.insertSurroundText("<s>", "</s>")
		}
	}

	/*************************************************
	 * Moment Button
	 *************************************************/
	Button {
		id: momentBtn
		anchors {
			left: palmipede.left
			leftMargin: 2
			top: boldBtn.bottom
			topMargin: 2
		}
		height: palmipede.lineHeight

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
		height: palmipede.lineHeight

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
					left: palmipede.left
					leftMargin: 2
					right: postBtn.left
					rightMargin: 2
					top:palmipede.top
					topMargin: 2
//					verticalCenter: boldBtn.verticalCenter
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

	function addBoard(board, color, colorLight) {
		boardSelectorItems.append({ "name": board,
									  "color": "" + color,
									  "colorLight": "" + colorLight })
	}

	function switchBoard(board) {
	}

	function doPost () {
		post(boardSelector.currentText, postInput.text)
		postInput.text = ""
	}

	signal insertReplaceTextBoard(string board, string txt)
	onInsertReplaceTextBoard: {
		switchBoard(board)
		insertReplaceText(txt)
	}

	signal insertReplaceText(string txt)
	onInsertReplaceText: {
		var headIdx = Math.min(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var tailIdx = Math.max(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var a = postInputTF.text.slice(0, headIdx)
		var c = postInputTF.text.slice(tailIdx)

		console.log(a, txt, c)
		postInputTF.text = a + txt + c
	}

	signal insertSurroundText(string head, string tail)
	onInsertSurroundText : {
		var headIdx = Math.min(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var tailIdx = Math.max(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var a = postInputTF.text.slice(0, headIdx)
		var b = postInputTF.text.slice(headIdx, tailIdx)
		var c = postInputTF.text.slice(tailIdx)

		console.log(a, txt, c)
		postInputTF.text = a + head + b + tail + c
	}
}
