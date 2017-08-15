#pragma once

#include <QAbstractProxyModel>

class TreeNode;

class FlattenedProxyModel: public QAbstractProxyModel {
    Q_OBJECT
    enum TreeRoles {
        Indentation = Qt::UserRole + 1,
        HasChildren,
        IsExpanded,
        Hidden
    };
public:
    FlattenedProxyModel(QObject* parent= nullptr);
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &proxyIndex, int role) const override;
    bool setData(const QModelIndex &proxyIndex, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void onSourceDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onRowsRemoved(const QModelIndex& parent, int first, int last);

private:
    int flatten(QAbstractItemModel *model, QModelIndex parent = QModelIndex(), TreeNode* parentNode= nullptr);
    void toggleIsExpanded(int row, bool isExpanded);
    void doResetModel(QAbstractItemModel *sourceModel);

    TreeNode* findNodeByIndex(const QModelIndex& index) const;

    QList<TreeNode*> flattenedTree_;
};

