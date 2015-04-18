import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.XmlListModel 2.0

import org.moules.quteqoin 1.0


ApplicationWindow {
	id: root
	width: 400
	height: 600
	visible: true

	XmlListModel {
		id: xmlModel
		source: "http://euromussels.eu/?q=tribune.xml" //"http://www.mysite.com/feed.xml"
		query: "/board/post"

		XmlRole { name: "login"; query: "login/string()" }
		XmlRole { name: "message"; query: "message/string()" }
		XmlRole { name: "time"; query: "@time/string()" }
		XmlRole { name: "ua"; query: "info/string()" }
	}

	ColumnLayout {
		id: mainLayout
		anchors.fill: parent
		anchors.margins: 2
		spacing: 2

		RowLayout {
			spacing: 2

			ComboBox {
				id: tabCB
				Layout.fillWidth: true
				Layout.alignment: Qt.AlignVCenter
				model: [ "Main", "Slow", "ReadOnly" ]
			}

			Button {
				Layout.minimumWidth: 24
				Layout.maximumWidth: 24
				Layout.minimumHeight: 24
				Layout.maximumHeight: 24
				Layout.alignment: Qt.AlignVCenter

				iconSource: "/img/settings-icon.png"
			}
		}

		ListView {
			Layout.fillWidth: true
			Layout.fillHeight: true
			model: xmlModel
			delegate: Text {
				width: parent.width
				wrapMode: Text.Wrap
				function buildText()
				{
					var norloge = time.substring(time.length - 6);
					var textData = "<b>[" + norloge.slice(0, 2) +
							":" + norloge.slice(2, 4) +
							":" + norloge.slice(4, 6) + "]</b> ";

					if(login != "")
						textData +=  "<font color=\"#553333\">" + login + "</font> " + message;
					else
						textData +=  "<font color=\"#883333\"><i>" + ua.slice(0, 15) + "</i></font> " + message;
					text = textData;
				}

				Component.onCompleted: buildText()
			}
		}
		/*

		TextArea {
			id: textArea

			Layout.fillHeight: true
			Layout.fillWidth: true

			readOnly: true
			wrapMode: TextEdit.Wrap
		}

		DocumentHandler {
			id: document
			target: textArea
			cursorPosition: textArea.cursorPosition
			selectionStart: textArea.selectionStart
			selectionEnd: textArea.selectionEnd
		}

		*/

		QQmlPalmi {
			id: palmi
			Layout.fillWidth: true
		}
	}
}
