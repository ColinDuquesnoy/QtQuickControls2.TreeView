import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

ScrollView {
    id: control

    property var model
    property Component delegate

    clip: true

    ListView {
        id: listView

        anchors.fill: parent
        cacheBuffer: 10000
        model: control.model

        delegate: control.delegate
    }
}