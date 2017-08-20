#include <QtGui/QStandardItemModel>
#include <TreeViewModel.h>
#include "catch.hpp"

using namespace std;

QList<QStandardItem*> makeBasicStandardItemModel(QStandardItemModel* standardItemModel)
{
    QList<QStandardItem*> allItems;
    QStandardItem* root = new QStandardItem("Root"); allItems.append(root);
    QStandardItem* child1  = new QStandardItem("Child 1"); allItems.append(child1);
    QStandardItem* child1OfChild1 = new QStandardItem("Child 1 of Child 1"); allItems.append(child1OfChild1);
    QStandardItem* child2OfChild1 = new QStandardItem("Child 2 of Child 1"); allItems.append(child2OfChild1);
    child1->appendRow(child1OfChild1);
    child1->appendRow(child2OfChild1);
    QStandardItem* child2 = new QStandardItem("Child 2"); allItems.append(child2);
    QStandardItem* child1OfChild2 = new QStandardItem("Child 1 of Child 2"); allItems.append(child1OfChild2);
    child2->appendRow(child1OfChild2);
    QStandardItem* child3 = new QStandardItem("Child 3"); allItems.append(child3);
    root->appendRow(child1);
    root->appendRow(child2);
    root->appendRow(child3);
    standardItemModel->appendRow(root);

    return allItems;
}

SCENARIO("TreeViewModel can be created from a static pre-filled QStandardItemModel")
{
    TreeViewModel treeViewModel;

    GIVEN("A pre-filled QStandardItemModel") {
        unique_ptr<QStandardItemModel> standardItemModel = make_unique<QStandardItemModel>();
        QList<QStandardItem*> allItems = makeBasicStandardItemModel(standardItemModel.get());

        REQUIRE(treeViewModel.rowCount() == 0);

        WHEN("the standardItemModel is set as the source model of the tree view model") {
            treeViewModel.setSourceModel(standardItemModel.get());

            THEN("rowCount equals the number of items") {
                REQUIRE(treeViewModel.rowCount() == allItems.count());
            }

            AND_THEN("each item is in the right order") {
                for (int i = 0; i < allItems.count(); ++i) {
                    QModelIndex proxyIndex = treeViewModel.index(i);
                    QString display = treeViewModel.data(proxyIndex, Qt::DisplayRole).toString();

                    REQUIRE(display.toStdString() == allItems[i]->text().toStdString());
                }
            }

            AND_THEN("each item is in the correct initial state") {
                for (int i = 0; i < allItems.count(); ++i) {
                    QModelIndex proxyIndex = treeViewModel.index(i);
                    bool hasChildren = treeViewModel.data(proxyIndex, TreeViewModel::HasChildren).toBool();
                    bool isHidden = treeViewModel.data(proxyIndex, TreeViewModel::Hidden).toBool();
                    bool isExpanded = treeViewModel.data(proxyIndex, TreeViewModel::IsExpanded).toBool();

                    REQUIRE(hasChildren == allItems[i]->hasChildren());
                    REQUIRE(!isExpanded);
                    if (i == 0)
                        // only root is not hidden
                        REQUIRE(!isHidden);
                    else
                        REQUIRE(isHidden);
                }
            }
        }
    }
}

SCENARIO("TreeItem can be collapsed and expanded")
{
    GIVEN("A TreeViewModel with a root node and some child nodes") {
        TreeViewModel treeViewModel;
        unique_ptr<QStandardItemModel> standardItemModel = make_unique<QStandardItemModel>();
        QList<QStandardItem*> allItems = makeBasicStandardItemModel(standardItemModel.get());
        treeViewModel.setSourceModel(standardItemModel.get());

        QModelIndex rootIndex = treeViewModel.index(0);
        QModelIndex child1Index = treeViewModel.index(1);

        REQUIRE(!treeViewModel.data(rootIndex, TreeViewModel::IsExpanded).toBool());
        REQUIRE(!treeViewModel.data(child1Index, TreeViewModel::IsExpanded).toBool());
        REQUIRE(treeViewModel.data(child1Index, TreeViewModel::Hidden).toBool());

        WHEN("root is expanded") {
            treeViewModel.setData(rootIndex, true, TreeViewModel::IsExpanded);

            THEN("root.isExpanded is true") {
                REQUIRE(treeViewModel.data(rootIndex, TreeViewModel::IsExpanded).toBool());
            }

            AND_THEN("Child1 is still collapsed but visible") {
                REQUIRE(!treeViewModel.data(child1Index, TreeViewModel::IsExpanded).toBool());
                REQUIRE(!treeViewModel.data(child1Index, TreeViewModel::Hidden).toBool());
            }

            AND_WHEN("root is collapsed") {
                treeViewModel.setData(rootIndex, false, TreeViewModel::IsExpanded);

                THEN("Root isExpanded is false") {
                    REQUIRE(!treeViewModel.data(rootIndex, TreeViewModel::IsExpanded).toBool());
                }

                AND_THEN("Child1 is still collapsed but hidden") {
                    REQUIRE(!treeViewModel.data(child1Index, TreeViewModel::IsExpanded).toBool());
                    REQUIRE(treeViewModel.data(child1Index, TreeViewModel::Hidden).toBool());
                }
            }
        }
    }
}

SCENARIO("TreeItem can be inserted dynamically")
{
    GIVEN("A TreeViewModel with a root node and some child nodes") {
        TreeViewModel treeViewModel;
        unique_ptr<QStandardItemModel> standardItemModel = make_unique<QStandardItemModel>();
        QList<QStandardItem*> allItems = makeBasicStandardItemModel(standardItemModel.get());
        treeViewModel.setSourceModel(standardItemModel.get());

        QStandardItem* child1 = allItems[1];
        QString firstChildText = allItems[2]->text();
        QString secondChildText = allItems[3]->text();

        WHEN("a new item is inserted in the source model between two items") {
            QStandardItem* insertedItem = new QStandardItem("Inserted Item");
            child1->insertRow(1, insertedItem);

            THEN("count is incremented") {
                REQUIRE(treeViewModel.rowCount() == allItems.count() + 1);
            }

            AND_THEN("inserted item is at the correct position") {
                QModelIndex child1OfChild1Index = treeViewModel.index(2);
                QModelIndex insertedItemIndex = treeViewModel.index(3);
                QModelIndex child2OfChild1Index = treeViewModel.index(4);

                REQUIRE(treeViewModel.data(child1OfChild1Index, Qt::DisplayRole).toString().toStdString() == firstChildText.toStdString());
                REQUIRE(treeViewModel.data(insertedItemIndex, Qt::DisplayRole).toString().toStdString() == insertedItem->text().toStdString());
                REQUIRE(treeViewModel.data(child2OfChild1Index, Qt::DisplayRole).toString().toStdString() == secondChildText.toStdString());
            }
        }
    }
}