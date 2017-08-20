#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardItem>
#include <QDebug>
#include <QStyle>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QTreeView>
#include <QtCore/QSortFilterProxyModel>
#include "TreeViewModel.h"

class SortFilterProxyModel : public QSortFilterProxyModel
{
public:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override
    {
        // If sorting by file names column
        if (sortColumn() == 0) {
            QFileSystemModel *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
            bool asc = sortOrder() == Qt::AscendingOrder ? true : false;

            QFileInfo leftFileInfo  = fsm->fileInfo(left);
            QFileInfo rightFileInfo = fsm->fileInfo(right);


            // If DotAndDot move in the beginning
            if (sourceModel()->data(left).toString() == "..")
                return asc;
            if (sourceModel()->data(right).toString() == "..")
                return !asc;

            // Move dirs upper
            if (!leftFileInfo.isDir() && rightFileInfo.isDir()) {
                return !asc;
            }
            if (leftFileInfo.isDir() && !rightFileInfo.isDir()) {
                return asc;
            }
        }

        return QSortFilterProxyModel::lessThan(left, right);
    }
};

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
    fileSystemModel.sort(0);
    /*QModelIndex rootIndex = */fileSystemModel.setRootPath(QDir::currentPath());

    TreeViewModel standardItemTreeViewModel;
    standardItemTreeViewModel.setSourceModel(&standardItemModel);

    TreeViewModel fileSystemTreeViewModel;

    SortFilterProxyModel sortFilterProxyModel;
    sortFilterProxyModel.setDynamicSortFilter(true);
    sortFilterProxyModel.sort(0);
    sortFilterProxyModel.setSourceModel(&fileSystemModel);

    fileSystemTreeViewModel.setSourceModel(&sortFilterProxyModel);

    QTreeView tv;
    tv.setModel(&sortFilterProxyModel);
    tv.show();

    qmlApplicationEngine.rootContext()->setContextProperty("standardItemModel", &standardItemTreeViewModel);
    qmlApplicationEngine.rootContext()->setContextProperty("fileSystemModel", &fileSystemTreeViewModel);
    qmlApplicationEngine.load(QUrl("qrc:/main.qml"));

    return qtApp.exec();
}
