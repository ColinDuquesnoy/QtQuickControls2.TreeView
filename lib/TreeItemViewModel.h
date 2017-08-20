#pragma once

#include <QtCore/QAbstractProxyModel>
#include <QtCore/QList>
#include <QtCore/QModelIndex>
#include <QDebug>


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
    TreeItemViewModel(QModelIndex sourceIndex, QList<TreeItemViewModel*>& flattenedTree,
                      QMap<QModelIndex, bool>& expandedMap, QMap<QModelIndex, bool>& hiddenMap):
        TreeItemViewModel(nullptr, sourceIndex, flattenedTree, proxyModel_, expandedMap, hiddenMap)
    {
    }

    TreeItemViewModel(TreeItemViewModel* parent, QModelIndex sourceIndex, QList<TreeItemViewModel*>& flattenedTree, QAbstractProxyModel* model,
                      QMap<QModelIndex, bool>& expandedMap, QMap<QModelIndex, bool>& hiddenMap):
            parent_(parent),
            proxyModel_(model),
            sourceIndex_(sourceIndex),
            flattenedTree_(flattenedTree),
            isExpanded_(false),
            expandedMap_(expandedMap),
            hiddenMap_(hiddenMap)
    {
        if (parent) {
            isHidden_ = parent->isCollapsed() || parent->isHidden();;
            indent_ = parent->indent() + 1;
        }
        else {
            isHidden_ = false;
            indent_ = 0;
            flattenedTree.append(this);
        }

        isExpanded_ = expandedMap.value(sourceIndexAcrossProxyChain(sourceIndex), isExpanded_);
    }

    QPersistentModelIndex sourceIndexAcrossProxyChain(const QModelIndex& proxyIndex) const {
        QAbstractProxyModel* proxyModel = proxyModel_;
        QAbstractProxyModel* nextSubProxyModel = qobject_cast<QAbstractProxyModel*>(proxyModel->sourceModel());
        QModelIndex sourceIndex = proxyIndex;
        while (nextSubProxyModel != nullptr) {
            proxyModel = nextSubProxyModel;
            sourceIndex = proxyModel->mapToSource(sourceIndex);
            nextSubProxyModel = qobject_cast<QAbstractProxyModel*>(proxyModel->sourceModel());
        }
        return QPersistentModelIndex(sourceIndex);
    }

    TreeItemViewModel* parent() const
    {
        return parent_;
    }

    int indexOfChild(const TreeItemViewModel* child) const
    {
        if (childItems_.contains(const_cast<TreeItemViewModel*>(child)))
            return childItems_.indexOf(const_cast<TreeItemViewModel*>(child));
        return -1;
    }

    QModelIndex sourceIndex() const
    {
        return sourceIndex_;
    }

    TreeItemViewModel* addChild(QModelIndex index)
    {
        TreeItemViewModel* child = new TreeItemViewModel(
                this, index, flattenedTree_, proxyModel_, expandedMap_, hiddenMap_);

        int insertPoint = getLastChildRow() + 1;
        flattenedTree_.insert(insertPoint, child);
        childItems_.append(child);

        return child;
    }

    TreeItemViewModel* insertChild(int row, QModelIndex index)
    {
        if (childItems_.count() > row) {
            TreeItemViewModel* child = new TreeItemViewModel(
                    this, index, flattenedTree_, proxyModel_, expandedMap_, hiddenMap_);
            int insertPoint = childItems_[row]->getLastChildRow();
            flattenedTree_.insert(insertPoint, child);
            childItems_.insert(row, child);
            return child;
        }
        else
            addChild(index);
    }

    void setExpanded(bool expanded)
    {
        isExpanded_ = expanded;
        expandedMap_[sourceIndexAcrossProxyChain(sourceIndex_)] = expanded;

        QModelIndex proxyIndex = proxyModel_->mapFromSource(sourceIndex());
        emit proxyModel_->dataChanged(proxyIndex, proxyIndex);

        if (proxyModel_->sourceModel()->canFetchMore(sourceIndex()))
            proxyModel_->sourceModel()->fetchMore(sourceIndex());

        for(TreeItemViewModel* child: childItems_)
            child->setHidden(!isExpanded_);
    }

    void setHidden(bool hidden)
    {
        isHidden_ = hidden;
        hiddenMap_[sourceIndex_] = hidden;

        QModelIndex proxyIndex = proxyModel_->mapFromSource(sourceIndex());
        emit proxyModel_->dataChanged(proxyIndex, proxyIndex);

        for(TreeItemViewModel* child: childItems_)
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
        return proxyModel_->sourceModel()->hasChildren(sourceIndex());
    }

    int getLastChildRow()
    {
        if (childItems_.count() > 0) {
            TreeItemViewModel *lastChild = childItems_.last();
            return lastChild->getLastChildRow();
        }
        else
            return row();
    }

private:
    QPersistentModelIndex sourceIndex_;
    int indent_;
    bool isExpanded_;
    bool isHidden_;

    TreeItemViewModel* parent_;
    QAbstractProxyModel* proxyModel_;
    QList<TreeItemViewModel*>& flattenedTree_;
    QList<TreeItemViewModel*> childItems_;
    QMap<QModelIndex, bool>& expandedMap_;
    QMap<QModelIndex, bool>& hiddenMap_;
};
