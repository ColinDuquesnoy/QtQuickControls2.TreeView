#include "FlattenedProxyModel.h"
#include "TreeNode.h"
#include <QDebug>

FlattenedProxyModel::FlattenedProxyModel(QObject *parent) : QAbstractProxyModel(parent) {

}

void FlattenedProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);
    doResetModel(sourceModel);

    if (sourceModel != nullptr) {
        connect(sourceModel, &QAbstractItemModel::dataChanged, this, &FlattenedProxyModel::onSourceDataChanged);
        connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &FlattenedProxyModel::onRowsInserted);
        connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &FlattenedProxyModel::onRowsRemoved);
    }
}

void FlattenedProxyModel::doResetModel(QAbstractItemModel* sourceModel) {
    beginResetModel();
    flatten(sourceModel);
    endResetModel();
}

QModelIndex FlattenedProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid() || !flattenedTree_.count() < proxyIndex.row())
        return QModelIndex();
    return flattenedTree_[proxyIndex.row()]->sourceIndex();
}

QModelIndex FlattenedProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    TreeNode* n = findNodeByIndex(sourceIndex);
    if (n == nullptr)
        return QModelIndex();
    return createIndex(n->row(), 0);
}

int FlattenedProxyModel::columnCount(const QModelIndex &parent) const
{
    return sourceModel()->columnCount(mapToSource(parent));
}

int FlattenedProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return flattenedTree_.count();
    return 0;
}

QModelIndex FlattenedProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    else
        return createIndex(row, column);

}

QModelIndex FlattenedProxyModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

void FlattenedProxyModel::onSourceDataChanged(QModelIndex topLeft, QModelIndex bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

int FlattenedProxyModel::flatten(QAbstractItemModel *model, QModelIndex parent, TreeNode* parentNode)
{
    if (parentNode == nullptr) {
        qDeleteAll(flattenedTree_);
        flattenedTree_.clear();
    }

    if (model == nullptr)
        return 0;

    auto rows = model->rowCount(parent);

    for(int rowIndex = 0; rowIndex < rows; ++rowIndex) {
        QModelIndex index = model->index(rowIndex, 0, parent);

        TreeNode* node = nullptr;
        if (parentNode)
            node = parentNode->addChild(index);
        else
            node = new TreeNode(parentNode, index, flattenedTree_, this);

        if (node->hasChildren())
            flatten(model, index, node);
    }
}

QVariant FlattenedProxyModel::data(const QModelIndex &proxyIndex, int role) const {
    switch (role) {
        case Indentation:
            return flattenedTree_[proxyIndex.row()]->indent();
        case HasChildren:
            return flattenedTree_[proxyIndex.row()]->hasChildren();
        case IsExpanded:
            return flattenedTree_[proxyIndex.row()]->isExpanded();
        case Hidden:
            return flattenedTree_[proxyIndex.row()]->isHidden();
        default:
            return flattenedTree_[proxyIndex.row()]->sourceIndex().data(role);
    }
}

QHash<int, QByteArray> FlattenedProxyModel::roleNames() const {
    auto names = QAbstractItemModel::roleNames();
    names[Indentation] = "indentation";
    names[HasChildren] = "hasChildren";
    names[IsExpanded] = "isExpanded";
    names[Hidden] = "hidden";
    return names;
}

bool FlattenedProxyModel::setData(const QModelIndex &proxyIndex, const QVariant &value, int role) {
    switch (role) {
        case IsExpanded:
            toggleIsExpanded(proxyIndex.row(), value.toBool());
            break;
        default:
            QAbstractProxyModel::setData(proxyIndex, value, role);
    }
}

void FlattenedProxyModel::toggleIsExpanded(int row, bool isExpanded) {
    flattenedTree_[row]->setExpanded(isExpanded);
}

void FlattenedProxyModel::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    TreeNode* parentNode = findNodeByIndex(parent);

    if (parentNode == nullptr)
        return;

    int firstRow = 0;
    int lastRow = 0;

    for (int row = first; row < last + 1; ++row) {
        QModelIndex childIndex = parent.child(row, 0);
        TreeNode* n = parentNode->addChild(childIndex);
        if (row == first)
            firstRow = n->row();
        else if (row == last)
            lastRow = n->row();
    }
    qDebug() << "inserted rows into parent: " << parent.data(Qt::DisplayRole) << parentNode->row() << firstRow << lastRow;
    beginInsertRows(QModelIndex(), firstRow, lastRow);

    endInsertRows();
}

void FlattenedProxyModel::onRowsRemoved(const QModelIndex &parent, int first, int last) {
    qDebug() << "rows removed";
}

TreeNode *FlattenedProxyModel::findNodeByIndex(const QModelIndex& sourceIndex)  const
{
    for (TreeNode* n: flattenedTree_) {
        if (n->sourceIndex() == sourceIndex)
            return n;
    }
    return nullptr;
}
