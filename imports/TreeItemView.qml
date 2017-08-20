import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2


ItemDelegate {
    id: delegateRoot

    property Component item
    property Component arrow
    property int indentation: 32

    clip: true
    // collapsed items have a null height
    height: model.hidden ? 0 : implicitHeight
    // fill available width
    width: ListView.view.width

    onDoubleClicked: toggleIsExpanded()

    Behavior on height {
        NumberAnimation { duration: 66 }
    }

    function toggleIsExpanded() {
        if (model.hasChildren)
            model.isExpanded = !model.isExpanded
    }

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.leftMargin: parent.leftPadding
        anchors.rightMargin: parent.rightPadding
        anchors.topMargin: parent.topPadding
        anchors.bottomMargin: parent.bottomPadding

        Item {
            // this is what make the list look like a tree
            Layout.preferredWidth: model.indentation * delegateRoot.indentation
        }

        AbstractButton {
            id: arrowControl

            contentItem: delegateRoot.arrow != null ? delegateRoot.arrow.createObject(arrowControl, {"model": model}) : null

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