#include "TreeNode.h"
#include <QDebug>

TreeNode::TreeNode(QModelIndex sourceIndex, QList<TreeNode*>& flattenedTree, QAbstractProxyModel* model):
    TreeNode(nullptr, sourceIndex, flattenedTree, model)
{
}

TreeNode::TreeNode(TreeNode *parent, QModelIndex sourceIndex, QList<TreeNode*>& flattenedTree,
                   QAbstractProxyModel* model):
        parent_(parent),
        model_(model),
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
        qDebug() << flattenedTree.count() << sourceIndex.data(Qt::DisplayRole);
        flattenedTree.append(this);
    }
}

QModelIndex TreeNode::sourceIndex() const {
    return sourceIndex_;
}

TreeNode* TreeNode::addChild(QModelIndex index) {
    TreeNode* childNode = new TreeNode(this, index, flattenedTree_, model_);

    int insertPoint = getLastChildRow() + 1;

    qDebug() << insertPoint << index.data(Qt::DisplayRole);
    flattenedTree_.insert(insertPoint, childNode);
    childNodes_.append(childNode);

    return childNode;
}

void TreeNode::setExpanded(bool expanded) {
    isExpanded_ = expanded;

    QModelIndex proxyIndex = model_->mapFromSource(sourceIndex_);
    emit model_->dataChanged(proxyIndex, proxyIndex);

    if (model_->sourceModel()->canFetchMore(sourceIndex_))
        model_->sourceModel()->fetchMore(sourceIndex_);

    for(TreeNode* child: childNodes_)
        child->setHidden(!isExpanded_);
}

int TreeNode::row() {
    return flattenedTree_.indexOf(this);
}

int TreeNode::indent() const {
    return indent_;
}

bool TreeNode::isExpanded() const {
    return isExpanded_;
}

bool TreeNode::isCollapsed() {
    return !isExpanded_;
}

bool TreeNode::isHidden() const {
    return isHidden_;
}

void TreeNode::setHidden(bool hidden) {
    isHidden_ = hidden;

    QModelIndex proxyIndex = model_->mapFromSource(sourceIndex_);
    emit model_->dataChanged(proxyIndex, proxyIndex);

    for(TreeNode* child: childNodes_)
        child->setHidden(isHidden_ || !isExpanded_);
}

bool TreeNode::hasChildren() const {
    return model_->sourceModel()->hasChildren(sourceIndex_);
}

int TreeNode::getLastChildRow() {
    if (childNodes_.count() > 0) {
        TreeNode *lastChildNode = childNodes_.last();
        return lastChildNode->getLastChildRow();
    }
    else
        return row();
}
