import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id:root
    height: postInput.height

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        RowLayout {
            spacing: 2

            TextField {
                Layout.fillWidth: true
                id: postInput
                placeholderText: "coin ! coin !"
            }

            Button {
                id: postBtn
                text: qsTr("Post")
            }
        }
    }
}
