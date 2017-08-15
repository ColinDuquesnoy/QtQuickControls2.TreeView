import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2


ItemDelegate {
    id: delegateRoot

    property Component item
    property Component arrow
    property int indentation: 32

    clip: true

    // collapsed items have a null heigh
    height: model.hidden ? 0 : implicitHeight
    // fill available width
    width: ListView.view.width

    onDoubleClicked: toggleIsExpanded()

    Behavior on height {
        NumberAnimation { duration: 100 }
    }

    function toggleIsExpanded() {
        if (model.hasChildren)
            model.isExpanded = !model.isExpanded
    }

    contentItem: RowLayout {
        id: layout

        Item {
            // this is what make the list look like a tree
            Layout.preferredWidth: model.indentation * delegateRoot.indentation
        }

        AbstractButton {
            id: arrowControl

            contentItem: delegateRoot.arrow != null ? delegateRoot.arrow.createObject(arrowControl, {"model": model}) : null
            visible: model.hasChildren

            onClicked: toggleIsExpanded()

            Layout.fillHeight: true
        }

        Control {
            id: displayControl

            contentItem: delegateRoot.item.createObject(displayControl, {"model": model})

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: displayControl.contentItem.contextMenu.open()
    }
}