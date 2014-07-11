import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.moules.quteqoin 1.0


ApplicationWindow {
    id: root
    width: 400
    height: 600
    visible: true

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

        QQmlPalmi {
            id: palmi
            Layout.fillWidth: true
        }
    }
}
