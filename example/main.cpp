#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardItem>
#include <QDebug>
#include <QStyle>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QTreeView>
#include "TreeViewModel.h"

int main(int argc, char** argv) {
    QApplication qtApp(argc, argv);
    QQmlApplicationEngine qmlApplicationEngine;

    QStandardItemModel standardItemModel;
    QStandardItem root("Root");
    root.setIcon(qtApp.style()->standardIcon(QStyle::SP_DesktopIcon));
    QStandardItem child1("Child 1");
    QStandardItem child2("Child 2");
    QStandardItem child3("Child 3");
    QStandardItem child1OfChild1("Child 1 of Child 1");
    QStandardItem child2OfChild1("Child 2 of Child 1");
    QStandardItem child1OfChild2("Child 1 of Child 2");

    child1.appendRow(&child1OfChild1);
    child1.appendRow(&child2OfChild1);
    child2.appendRow(&child1OfChild2);
    root.appendRow(&child1);
    root.appendRow(&child2);
    root.appendRow(&child3);
    standardItemModel.appendRow(&root);

    QFileSystemModel fileSystemModel;
//    QModelIndex rootIndex = fileSystemModel.setRootPath(QDir::currentPath());

    TreeViewModel standardItemTreeViewModel;
    standardItemTreeViewModel.setSourceModel(&standardItemModel);

    TreeViewModel fileSystemTreeViewModel;
    fileSystemTreeViewModel.setSourceModel(&fileSystemModel);
//    fileSystemTreeViewModel.setRootIndex(rootIndex);

    qmlApplicationEngine.rootContext()->setContextProperty("standardItemModel", &standardItemTreeViewModel);
    qmlApplicationEngine.rootContext()->setContextProperty("fileSystemModel", &fileSystemTreeViewModel);
    qmlApplicationEngine.load(QUrl("qrc:/main.qml"));

    return qtApp.exec();
}
