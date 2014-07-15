import QtQuick 2.0

Rectangle {
	SystemPalette { id: activePalette }

	id: mainRectangle
	width: 500
	height: 600
	color: activePalette.window

	// Put the name of the QML files containing your pages (without the '.qml')
	property variant pagesList  : [
		"QQmlGeneralSettings",
		"QQmlTotozSettings",
		"Page3",
		"Page4",
		"Page5"
	];

	Rectangle {
		id: rectangleMenu
		width: 120
		color: activePalette.base
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 4
		anchors.top: parent.top
		anchors.topMargin: 4
		anchors.left: parent.left
		anchors.leftMargin: 4
		border.width: 1
		radius: 5
		border.color: activePalette.dark

		Column {
			id: column1
			anchors.fill: parent
			anchors.topMargin: 4
			anchors.leftMargin: 4
			anchors.rightMargin: 4
			spacing: 6

			QQmlSettingsItemMenuBtn {
				id: generalSettings
				hoverColor: activePalette.alternateBase
				componentName: pagesList[0]
				width: parent.width
				btnImg: "../img/general-icon.png"
				btnTxt: qsTr("General")
			}

			QQmlSettingsItemMenuBtn {
				id: totozSettings
				hoverColor: activePalette.alternateBase
				componentName: pagesList[1]
				width: parent.width
				btnImg: "../img/totoz-icon.jpeg"
				btnTxt: qsTr("Totoz")
			}

			QQmlSettingsItemMenuBtn {
				id: boardSettings
				hoverColor: activePalette.alternateBase
				width: parent.width
				btnImg: "../img/board-icon.png"
				btnTxt: qsTr("Boards")
			}

			QQmlSettingsItemMenuBtn {
				hoverColor: activePalette.alternateBase
				width: parent.width
				btnImg: "../img/palmipede-icon.png"
				btnTxt: qsTr("Palmipede")
			}

			QQmlSettingsItemMenuBtn {
				hoverColor: activePalette.alternateBase
				width: parent.width
				btnImg: "../img/filter-icon.png"
				btnTxt: qsTr("Filters")
			}

			QQmlSettingsItemMenuBtn {
				hoverColor: activePalette.alternateBase
				width: parent.width
				btnImg: "../img/hunt-icon.png"
				btnTxt: qsTr("Hunting")
			}
		}
	}

	Rectangle {
		id: rectangleData
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 4
		anchors.top: parent.top
		anchors.topMargin: 4
		anchors.left: rectangleMenu.right
		anchors.leftMargin: 4
		anchors.right: parent.right
		anchors.rightMargin: 4
		color: activePalette.window

		// Set this property to another file name to change page
		property string currentPage : "QQmlGeneralSettings";

		Repeater {
			id: repeater1
			model: mainRectangle.pagesList;
			delegate: Loader {
				active: false;
				asynchronous: true;
				anchors.fill: parent;
				source: "%1.qml".arg(modelData)
				/*
				Component.onCompleted: {
					loadIfNotLoaded();
				}
				*/

				function loadIfNotLoaded() {
					if (!active) {
						active = true;
					}
				}
			}
		}

		onCurrentPageChanged: {
			for(var i = 0; i < mainRectangle.pagesList.length; i++) {
				if(repeater1.itemAt(i).item) {
					repeater1.itemAt(i).item.state = "hidden"
				}
			}

			var index = mainRectangle.pagesList.indexOf(currentPage)
			var selecteditem = repeater1.itemAt(index).item
			console.log("1: ", index, selecteditem)
			if(!selecteditem) {
				repeater1.itemAt(index).loadIfNotLoaded()
				selecteditem = repeater1.itemAt(index).item
			}
			console.log("2: ", index, selecteditem)
			selecteditem.state = "visible"
		}
	}


	/*

	Flipable {
		id: flipable1
		anchors.top: parent.top
		anchors.topMargin: 4
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 4
		anchors.left: menuRect1.right
		anchors.leftMargin: 4
		anchors.right: parent.right
		anchors.rightMargin: 4

		property bool flipped: false

		transform: Rotation {
			id: rotation
			origin.x: flipable1.width/2
			origin.y: flipable1.height/2
			axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
			angle: 0    // the default angle
		}

		states: State {
			name: "back"
			PropertyChanges { target: rotation; angle: 180 }
			when: flipable1.flipped
		}

		transitions: Transition {
			NumberAnimation { target: rotation; property: "angle"; easing.type: Easing.OutCubic; duration: 1000 }
		}
	}
	*/

	/*
	function showPanel(name) {
		var comp = Qt.createComponent(name);
		if (comp.status === Component.Ready) {
			var obj  = comp.createObject(flipable1, {"anchors.fill": flipable1});
			if(flipable1.flipped)
				flipable1.front = obj
			else
				flipable1.back = obj
			flipable1.flipped = !flipable1.flipped;
			console.log("OK " + name)
		}
		else
			console.log("KO " + name)
	}
	*/

	function showPanel(name) {
		rectangleData.currentPage = name;
	}

	Component.onCompleted: {
		generalSettings.trigger.connect(showPanel);
		totozSettings.trigger.connect(showPanel);
		boardSettings.trigger.connect(showPanel);

		generalSettings.activate();
	}
}
