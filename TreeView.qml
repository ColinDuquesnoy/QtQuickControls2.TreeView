import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import TreeView 1.0

ScrollView {
    id: control

    property var model
    property Component delegate

    clip: true

    ListView {
        id: listView

        anchors.fill: parent
        model: d.proxyModel

        delegate: control.delegate
    }

    QtObject {
        id: d

        property var proxyModel: FlattenedProxyModel { sourceModel: control.model }
    }
}