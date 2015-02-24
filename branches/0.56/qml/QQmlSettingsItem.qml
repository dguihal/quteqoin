import QtQuick 2.0

Rectangle {
	id:rectangle1
	width: 100
	height: 62

	state: "hidden"

	states: [
		State { name: "hidden"; PropertyChanges { target: rectangle1; opacity: 0.0 } },
		State { name: "visible"; PropertyChanges { target: rectangle1; opacity: 1.0 } }
	]

	transitions: [
		Transition {
			from: "*"
			to: "hidden"
			PropertyAnimation { target: rectangle1; property: "opacity"; duration: 1000 }
		},
		Transition {
			from: "*"
			to: "visible"
			PropertyAnimation { target: rectangle1; property: "opacity"; duration: 1000 }
		}
	]
}
