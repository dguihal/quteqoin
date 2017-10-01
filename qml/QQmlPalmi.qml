import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
	id: palmipede
	height: boards.childrenRect.height + postInput.height + fmtRow.height + 6
	width: 400

	property int lineHeight: 30
	property string currentBoardName: ""

	SystemPalette {
		id: myPalette
		colorGroup: SystemPalette.Active
	}
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
			delegate: QQmlPalmiBoardItem {
				name: boardName
				colorDark: boardColor
				colorLight: boardColorLight
				onClicked: palmipede.switchBoard(boardName)
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

		FocusScope {
			id: tfFocusScope

			anchors.fill: parent
			focus: true

			TextField {
				id: postInputTF

				anchors.fill: parent
				placeholderText: "coin ! coin !"
				focus: true

				background: Rectangle {
					color: "transparent"
				}

				color: myPalette.text
				cursorVisible: true
				selectByMouse: true

				Keys.onUpPressed: {
					if (event.modifiers & Qt.AltModifier) {
						for (var i = 0; i < boardListModel.count; i++) {
							var boardElt = boardListModel.get(i)
							if (boardElt.boardName === currentBoardName) {
								var newBoard = boardListModel.get(
											(i + 1) % boardListModel.count)
								switchBoard(newBoard.boardName)
								break
							}
						}
					}
				}
				Keys.onDownPressed: {
					if (event.modifiers & Qt.AltModifier) {
						for (var i = 0; i < boardListModel.count; i++) {
							var boardElt = boardListModel.get(i)
							if (boardElt.boardName === currentBoardName) {
								var newBoard = boardListModel.get(
											((i - 1) % boardListModel.count
											 + boardListModel.count) % boardListModel.count)
								switchBoard(newBoard.boardName)
								break
							}
						}
					}
				}

				onAccepted: palmipede.doPost()
			}
		}

		property alias text: postInputTF.text

		Component.onCompleted: {
			if (currentBoardName != '') {
				var i = 0
				for (i; i < boardListModel.count; i++) {
					var boardElt = boardListModel.get(i)
					if (boardElt.boardName === currentBoardName) {
						postInput.color = boardElt.colorLight
						postInput.border.color = boardElt.color
					}
				}
			}
		}
	}
	/*************************************************
	 * Post Button
	 *************************************************/
	Button {
		id: postBtn
		anchors {
			right: attachBtn.left
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}
		text: qsTr("📧")
		onClicked: palmipede.doPost()

		background: Rectangle {
			//implicitWidth: Math.max(height, postBtnTxt.contentWidth + 4)
			implicitHeight: palmipede.lineHeight
			opacity: enabled ? 1 : 0.3
			border.width: 1
			radius: 4
		}
	}

	/*************************************************
	 * Attach Button
	 *************************************************/
	Button {
		id: attachBtn
		anchors {
			right: palmipede.right
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}

		contentItem: Image {
			id: attachBtnImg
			anchors {
				centerIn: parent
			}
			source: "qrc:/img/attach.png"
		}

		//onClicked: palmipede.doAttach()
		background: Rectangle {
			//implicitWidth: Math.max(height, postBtnTxt.contentWidth + 4)
			implicitHeight: palmipede.lineHeight
			opacity: enabled ? 1 : 0.3
			border.width: 1
			radius: 4
		}
	}

	/*************************************************
	 * Text formating
	 *************************************************/
	Row {
		id: fmtRow
		anchors {
			right: palmipede.right
			rightMargin: 2
			top: postInput.bottom
			topMargin: 2
		}
		layoutDirection: Qt.LeftToRight
		width: childrenRect.width
		spacing: 1

		property int fontPtSize: 9

		Text {
			text: "Click for :"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: boldBtn
			borderColor: myPalette.text
			text: "Bold"
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  bold: true,
								  pointSize: fmtRow.fontPtSize
							  })
		}
		Text {
			text: "|"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: italicBtn
			borderColor: myPalette.text
			text: "Italic"
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  italic: true,
								  pointSize: fmtRow.fontPtSize
							  })
		}
		Text {
			text: "|"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: underlineBtn
			borderColor: myPalette.text
			text: "Underline"
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  underline: true,
								  pointSize: fmtRow.fontPtSize
							  })
		}
		Text {
			text: "|"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: strikeBtn
			borderColor: myPalette.text
			text: "Strike"
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  strikeout: true,
								  pointSize: fmtRow.fontPtSize
							  })
		}
		Text {
			text: "|"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: momentBtn
			borderColor: myPalette.text
			text: "=> Moment <="
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  bold: true,
								  pointSize: fmtRow.fontPtSize
							  })
		}
		Text {
			text: "|"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: pafBtn
			borderColor: myPalette.text
			text: "Paf !"
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  bold: true,
								  pointSize: fmtRow.fontPtSize
							  })
		}
		Text {
			text: "|"
			font.pointSize: fmtRow.fontPtSize
			color: myPalette.buttonText
			padding: 1
		}
		QQmlPalmiEditItem {
			id: blamBtn
			borderColor: myPalette.text
			text: "Blam !"
			textColor: myPalette.buttonText
			textFont: Qt.font({
								  bold: true,
								  pointSize: fmtRow.fontPtSize
							  })
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
	Component.onCompleted: {
		boldBtn.clicked.connect(onBoldClicked)
		italicBtn.clicked.connect(onItalicClicked)
		underlineBtn.clicked.connect(onUnderlineClicked)
		strikeBtn.clicked.connect(onStrikeClicked)
		momentBtn.clicked.connect(onMomentClicked)
		blamBtn.clicked.connect(onBlamClicked)
		pafBtn.clicked.connect(onPafClicked)
	}

	onCurrentBoardNameChanged: {
		console.log("onCurrentBoardNameChanged", currentBoardName)
		var i = 0
		for (i; i < boardListModel.count; i++) {
			var boardElt = boardListModel.get(i)
			if (boardElt.boardName === currentBoardName) {
				postInput.color = boardElt.boardColorLight
				postInput.border.color = boardElt.boardColor
			}
		}
	}

	function addBoard(board, color, colorLight) {
		boardListModel.append({
								  boardName: board,
								  boardColor: "" + color,
								  boardColorLight: "" + colorLight
							  })
		if (currentBoardName === "") {
			switchBoard(board)
		}
	}

	function removeBoard(board) {
		for (var i = 0; i < boardListModel.count; i++) {
			var boardElt = boardListModel.get(i)
			console.log(board, boardElt.boardName)
			if (boardElt.boardName === board) {
				boardListModel.remove(i)
				if (currentBoardName === board) {
					boardElt = boardListModel.get(i % boardListModel.count)
					console.log(board, boardElt.boardName)
					switchBoard(boardElt.boardName)
				}
				break
			}
		}
	}

	function switchBoard(board) {
		for (var i = 0; i <= boardListModel.count; i++) {
			var b = boardListModel.get(i)
			if (b.boardName === board) {
				postInput.color = b.boardColorLight
				postInput.border.color = b.boardColor
				currentBoardName = board
				//postInput.forceActiveFocus(Qt.MouseFocusReason)
				break
			}
		}
	}

	signal post(string bouchot, string message)
	function doPost() {
		post(currentBoardName, postInput.text)
		postInput.text = ""
	}

	signal insertReplaceTextBoard(string board, string txt)
	onInsertReplaceTextBoard: {
		switchBoard(board)
		insertReplaceText(txt)
	}

	signal insertText(string txt)
	onInsertText: {
		var idx = txt.indexOf("%s")
		if (idx >= 0) {
			var head = txt.slice(0, idx)
			var tail = txt.slice(idx + 2)

			insertSurroundText(head, tail)
		} else {
			insertReplaceText(txt)
		}
	}

	function insertReplaceText(txt) {
		var headIdx = Math.min(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var tailIdx = Math.max(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var a = postInputTF.text.slice(0, headIdx)
		var c = postInputTF.text.slice(tailIdx)

		postInputTF.text = a + txt + c
	}

	function insertSurroundText(head, tail) {
		var headIdx = Math.min(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var tailIdx = Math.max(postInputTF.selectionStart,
							   postInputTF.selectionEnd)
		var a = postInputTF.text.slice(0, headIdx)
		var b = postInputTF.text.slice(headIdx, tailIdx)
		var c = postInputTF.text.slice(tailIdx)

		postInputTF.text = a + head + b + tail + c

		if (headIdx != tailIdx) {
			// Texte selectionne
			postInputTF.cursorPosition = postInputTF.text.length
		} else {
			postInputTF.cursorPosition = headIdx + head.length
		}
	}

	function onBoldClicked() {
		insertSurroundText("<b>", "</b>")
	}

	function onItalicClicked() {
		insertSurroundText("<i>", "</i>")
	}

	function onUnderlineClicked() {
		insertSurroundText("<u>", "</u>")
	}

	function onStrikeClicked() {
		insertSurroundText("<s>", "</s>")
	}

	function onMomentClicked() {
		insertSurroundText("====> <b>Moment ", "</b> <====")
	}

	function onPafClicked() {
		insertReplaceText("_o/* <b>paf!</b> ")
	}

	function onBlamClicked() {
		insertReplaceText("_o/* <b>BLAM</b>! ")
	}
}
