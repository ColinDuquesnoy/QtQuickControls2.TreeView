#pragma once

#include <QtCore/QAbstractProxyModel>
#include <QtCore/QList>
#include <QtCore/QModelIndex>


/**
 * @brief The class TreeItemViewModel is an internal representation of a tree item for use by the TreeViewModel.
 *
 * It contains the logic for adding and removing child nodes and updating the flat list representation of the tree
 * and holds the TreeItemView specific roles such as isExpanded, isHidden,...
 */
class TreeItemViewModel
{
public:
    /**
     * Creates a root item.
     *
     * @param sourceIndex sourceIndex of the model
     * @param flattenedTree Flat representation of the tree
     * @param proxyModel The proxy model that contains the items.
     */
    TreeItemViewModel(QModelIndex sourceIndex, QList<TreeItemViewModel*>& flattenedTree, QAbstractProxyModel* proxyModel):
        TreeItemViewModel(nullptr, sourceIndex, flattenedTree, proxyModel)
    {
    }

    TreeItemViewModel(TreeItemViewModel* parent, QModelIndex sourceIndex, QList<TreeItemViewModel*>& flattenedTree, QAbstractProxyModel* model):
            parent_(parent),
            proxyModel(model),
            sourceIndex_(sourceIndex),
            flattenedTree_(flattenedTree),
            isExpanded_(false)
    {

        if (parent) {
            isHidden_ = parent->isCollapsed() || parent->isHidden();
            indent_ = parent->indent() + 1;
        }
        else {
            isHidden_ = false;
            indent_ = 0;
            flattenedTree.append(this);
        }
    }

    QModelIndex sourceIndex() const
    {
        return sourceIndex_;
    }

    TreeItemViewModel* addChild(QModelIndex index)
    {
        TreeItemViewModel* child = new TreeItemViewModel(this, index, flattenedTree_, proxyModel);

        int insertPoint = getLastChildRow() + 1;
        flattenedTree_.insert(insertPoint, child);
        childItems.append(child);

        return child;
    }

    void setExpanded(bool expanded)
    {
        isExpanded_ = expanded;

        QModelIndex proxyIndex = proxyModel->mapFromSource(sourceIndex_);
        emit proxyModel->dataChanged(proxyIndex, proxyIndex);

        if (proxyModel->sourceModel()->canFetchMore(sourceIndex_))
            proxyModel->sourceModel()->fetchMore(sourceIndex_);

        for(TreeItemViewModel* child: childItems)
            child->setHidden(!isExpanded_);
    }

    void setHidden(bool hidden)
    {
        isHidden_ = hidden;

        QModelIndex proxyIndex = proxyModel->mapFromSource(sourceIndex_);
        emit proxyModel->dataChanged(proxyIndex, proxyIndex);

        for(TreeItemViewModel* child: childItems)
            child->setHidden(isHidden_ || !isExpanded_);
    }

    int row()
    {
        return flattenedTree_.indexOf(this);
    }

    int indent() const
    {
        return indent_;
    }

    bool isExpanded() const
    {
        return isExpanded_;
    }

    bool isCollapsed()
    {
        return !isExpanded_;
    }

    bool isHidden() const
    {
        return isHidden_;
    }

    bool hasChildren() const
    {
        return proxyModel->sourceModel()->hasChildren(sourceIndex_);
    }

    int getLastChildRow()
    {
        if (childItems.count() > 0) {
            TreeItemViewModel *lastChild = childItems.last();
            return lastChild->getLastChildRow();
        }
        else
            return row();
    }

private:
    QModelIndex sourceIndex_;
    int indent_;
    bool isExpanded_;
    bool isHidden_;

    TreeItemViewModel* parent_;
    QAbstractProxyModel* proxyModel;
    QList<TreeItemViewModel*>& flattenedTree_;
    QList<TreeItemViewModel*> childItems;
};
