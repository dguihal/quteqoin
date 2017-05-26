import QtQuick 2.6
import QtQuick.Controls 2.0

Rectangle {
	id:palmipede
	height: boards.childrenRect.height + postInput.height + boldBtn.height + 6
	width: 400

	property int lineHeight: 30
	property string currentBoardName: ""

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }
    color: myPalette.window

	ListModel {
		id: boardListModel
	}

	///////////////////////////////////////////////////
	Flow {
		id: boards

		anchors {
			left: palmipede.left
			leftMargin: 2
			right: palmipede.right
			rightMargin: 2
			top: palmipede.top
			topMargin: 2
		}

		Repeater {
			model: boardListModel
			delegate: Rectangle {
				width: boardTxt.contentWidth
				height: boardTxt.contentHeight

				radius: 4
				border.width: 1
				border.color: "darkgray"

				color: boardColorLight
				Text {
					id: boardTxt
                    font.pixelSize: 10
					text: boardName
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						postInput.color = boardColorLight
						postInput.border.color = boardColor
						currentBoardName = boardName
					}
				}
			}
		}

		onChildrenChanged: {
			palmipede.update()
		}

		spacing: 1
	}

	///////////////////////////////////////////////////

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
			top: boards.bottom //palmipede.top
			topMargin: 2
		}
        border.color: myPalette.text
		height: palmipede.lineHeight
		radius: 4

		TextField {
			id: postInputTF
			anchors.fill: parent
			placeholderText: "coin ! coin !"

            background: Rectangle {
                color: "transparent"
            }

            color: "black"
            cursorVisible: true
            selectByMouse: true

			Keys.onUpPressed: {
				if(event.modifiers & Qt.AltModifier)
				{
					var i = 0;
					for(i; i < boardListModel.count; i++)
					{
						var boardElt = boardListModel.get(i)
						if(boardElt.name === text)
						{
							var newBoard = boardListModel.get(i + 1)
							postInput.color = newBoard.colorLight
							postInput.border.color = newBoard.color
							currentBoardName = newBoard.boardName

						}
					}
				}
			}

			onAccepted: palmipede.doPost()
		}

		property alias text: postInputTF.text

		Component.onCompleted: {
			if(currentBoardName != '')
			{
				var i = 0;
				for(i; i < boardListModel.count; i++)
				{
					var boardElt = boardListModel.get(i)
					if(boardElt.name === currentBoardName)
					{
						postInput.color = boardElt.colorLight
						postInput.border.color = boardElt.color
					}
				}
			}
		}
	}
	/*************************************************
	 * Board Selector
	 *************************************************/

	/*
	Rectangle {
		id: boardSelector
		anchors {
			right: postBtn.left
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}
		width: boardTxt.contentWidth + 4
		height: palmipede.lineHeight
		radius: 4

		Text
		{
			id: boardTxt
			anchors {
				centerIn: parent
				margins: 2
			}
		}

		MouseArea {
			id: boardMA

			anchors.fill: parent
			onClicked: boardMenu.popup()
		}

		Menu {
			id:boardMenu
			visible: false

			Instantiator {
				model: boardListModel
				MenuItem {
					text: model.boardName
					onTriggered: {
						var i = 0;
						for(i; i < boardListModel.count; i++)
						{
							var boardElt = boardListModel.get(i)
							if(boardElt.name === text)
							{
								boardMenu.doBoadSelected(boardElt)
								break
							}
						}
					}
				}
				onObjectAdded: boardMenu.insertItem(index, object)
				onObjectRemoved: boardMenu.removeItem(object)
			}

			onItemsChanged: {
				if(boardTxt.text === "")
				{
					doBoadSelected(boardListModel.get(0))
				}
			}

			function doBoadSelected(boardElt)
			{
				boardTxt.text = boardElt.boardName
				boardSelector.color = boardElt.boardColor
				postInput.color = boardElt.boardColorLight
				postInput.border.color = boardElt.boardColor
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
        border.color: myPalette.buttonText
        color: myPalette.button
		height: palmipede.lineHeight
		radius: 4
        width: Math.max(height, postBtnTxt.contentWidth + 4)

		Text {
			id: postBtnTxt
			anchors.centerIn: parent
            text: qsTr("📧")
            color: myPalette.buttonText
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
        border.color: myPalette.text
        color: myPalette.button
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
        border.color: myPalette.text
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
				pointSize: 12
			}
            color: myPalette.text
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
        border.color: myPalette.text
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
				pointSize: 12
			}
            color: myPalette.text
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
        border.color: myPalette.text
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
				pointSize: 12
			}
            color: myPalette.text
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
        border.color: myPalette.text
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
				pointSize: 12
			}
            color: myPalette.text
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
	Rectangle {
		id: momentBtn
		anchors {
			left: strikeBtn.right
			leftMargin: 2
			top: strikeBtn.top
			bottom: strikeBtn.bottom
        }
        border.color: myPalette.text
		color: "transparent"
		radius: 4
		width: Math.max(height, momentBtnTxt.contentWidth + 4)

		Text {
			id: momentBtnTxt

			anchors.centerIn: parent
			text: qsTr("====> Moment < ====")
			font {
				family: "Times New Roman"
				bold: true
				pointSize: 12
			}
            color: myPalette.text
		}

		MouseArea {
			id: momentBtnMA

			anchors.fill: parent
			onPressed: palmipede.insertSurroundText("====> <b>Moment ", "</b> <====")
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

        delegate: ItemDelegate {
            width: blamPafSelector.width
            font: blamPafSelector.font
            contentItem: Text {
                text: modelData
                color: myPalette.text
                elide: Text.ElideRight
                verticalAlignment: Text.AlignLeft
            }
            highlighted: myPalette.highlightedIndex == index
        }

        font.bold: true

        model: [ "_o/* BLAM!", "_o/* paf!"]

        contentItem: Text {
            leftPadding: 0
            rightPadding: blamPafSelector.indicator.width + blamPafSelector.spacing

            text: blamPafSelector.displayText
            font: blamPafSelector.font
            color: myPalette.text
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            border.color: myPalette.text
            color: myPalette.window
            radius: 2
        }

        popup: Popup {
            y: blamPafSelector.height - 1
            width: blamPafSelector.width
            implicitHeight: listview.contentHeight
            padding: 1

            contentItem: ListView {
                id: listview
                clip: true
                model: blamPafSelector.popup.visible ? blamPafSelector.delegateModel : null
                currentIndex: blamPafSelector.highlightedIndex

                ScrollIndicator.vertical: ScrollIndicator { }
            }

            background: Rectangle {
                border.color: myPalette.windowText
                radius: 2
            }
        }
	}

/*
	states: [
		State {
			name: "MAXIMIZED"
			PropertyChanges {
				target: palmipede
				height: 98
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
					left: palmipede.left
					leftMargin: 2
					right: boardSelector.left
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
	*/

	onCurrentBoardNameChanged: {
		console.log("onCurrentBoardNameChanged", currentBoardName);
		var i = 0;
		for(i; i < boardListModel.count; i++)
		{
			var boardElt = boardListModel.get(i)
			if(boardElt.name === currentBoardName)
			{
				postInput.color = boardElt.colorLight
				postInput.border.color = boardElt.color
			}
		}
	}

	signal post (string bouchot, string message)

	function addBoard(board, color, colorLight) {
		boardListModel.append({ "boardName": board,
								  "boardColor": "" + color,
								  "boardColorLight": "" + colorLight })
		if(currentBoardName === '')
			currentBoardName = board;
	}

	function switchBoard(board) {
		currentBoardName = board
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

		postInputTF.text = a + head + b + tail + c
	}
}
