import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

import "qrc:/../imports"

ApplicationWindow {
    id: window
    width: 800; height: 600
    title: "TreeView Example"
    visible: true

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8

        GroupBox {
            title: "StandardItemModel"

            Layout.fillHeight: true
            Layout.preferredWidth: window.width / 2 - 16

            TreeView {
                anchors.fill: parent
                model: standardItemModel
                delegate: TreeItemView {
                    arrow: Label {
                        font.family: "monospace"
                        text: ">"
                        rotation: model.isExpanded ? 90 : 0
                        verticalAlignment: Label.AlignVCenter
                    }

                    item: Label {
                        property var contextMenu: Menu {
                            y: parent.implicitHeight

                            MenuItem {
                                text: "Delete"
                            }

                            MenuItem {
                                text: "Add new"
                            }
                        }

                        text: model.display
                        verticalAlignment: Label.AlignVCenter
                    }
                }
            }
        }

        GroupBox {
            title: "FileSystemModel"

            Layout.fillHeight: true
            Layout.preferredWidth: window.width / 2 - 16

            TreeView {
                anchors.fill: parent
                model: fileSystemModel

                delegate: TreeItemView {
                    arrow: Label {
                        font.family: "monospace"
                        text: model.hasChildren ? ">" : " "
                        rotation: model.isExpanded ? 90 : 0
                        verticalAlignment: Label.AlignVCenter
                    }

                    item: Label {
                        property var contextMenu: Menu {
                            y: parent.implicitHeight

                            MenuItem {
                                text: "Delete"
                            }

                            MenuItem {
                                text: "Add new"
                            }
                        }

                        text: model.display
                        verticalAlignment: Label.AlignVCenter
                    }
                }
            }
        }
    }
}