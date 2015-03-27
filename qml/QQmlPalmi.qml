import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Rectangle {
	id:palmipede
	height: 94
	width: 400

	TextField {
		id: postInput
		height: 30
		anchors {
			left: palmipede.left
			leftMargin: 2
			right: boardSelector.left
			rightMargin: 2
		}

		placeholderText: "coin ! coin !"
		onAccepted: {
			palmipede.post();
		}
	}

	ComboBox {
		id: boardSelector
		anchors {
			right: postBtn.left
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}

		model: [ "" ]
	}

	Button {
		id: postBtn
		anchors {
			right: attachBtn.left
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}

		text: qsTr("Post")
		style: ButtonStyle {
			label: Text {
			  renderType: Text.NativeRendering
			  verticalAlignment: Text.AlignVCenter
			  horizontalAlignment: Text.AlignHCenter
			  text: postBtn.text
			}
		}

		onClicked: {
			palmipede.post();
		}
	}

	Button {
		id: attachBtn
		width: height
		anchors {
			right: palmipede.right
			rightMargin: 2
			verticalCenter: postInput.verticalCenter
		}
		style: ButtonStyle {
		}

		Image {
			anchors.fill: parent
			source: "qrc:/img/attach.png"
			fillMode: Image.PreserveAspectFit
		}
	}

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
					right: boardSelector.left
					rightMargin: 2
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

	function post() {

	}
}
