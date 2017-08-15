#pragma once

#include <QtCore/QAbstractProxyModel>
#include <QtCore/QList>
#include <QtCore/QModelIndex>

class TreeNode
{
public:
    TreeNode(QModelIndex sourceIndex, QList<TreeNode*>& flattenedTree, QAbstractProxyModel* model);
    TreeNode(TreeNode* parent, QModelIndex sourceIndex, QList<TreeNode*>& flattenedTree, QAbstractProxyModel* model);

    QModelIndex sourceIndex() const;

    TreeNode* addChild(QModelIndex index);

    void setExpanded(bool expanded);
    void setHidden(bool hidden);

    int row();
    int indent() const;
    bool isExpanded() const;
    bool isCollapsed();
    bool isHidden() const;
    bool hasChildren() const;

private:
    int getLastChildRow();

    QModelIndex sourceIndex_;
    int indent_;
    bool isExpanded_;
    bool isHidden_;

    TreeNode* parent_;
    QAbstractProxyModel* model_;
    QList<TreeNode*>& flattenedTree_;
    QList<TreeNode*> childNodes_;
};
