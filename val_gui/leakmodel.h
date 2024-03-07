#ifndef LEAKMODEL_H
#define LEAKMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QVariant>
#include "leak.h"

class LeakTreeModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit LeakTreeModel(const QList<Leak>& leaks, QObject *parent = nullptr)
        : QAbstractItemModel(parent), rootItem("Root") {
        // Configuration des données de fuites
        for (const auto& leak : leaks)
            rootItem.addChild(new LeakTreeItem(leak));
    }

    // Implémentation des méthodes virtuelles pures de QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        LeakTreeItem *parentItem = getItem(parent);

        LeakTreeItem *childItem = parentItem->child(row);
        if (childItem)
            return createIndex(row, column, childItem);
        else
            return QModelIndex();
    }

    QModelIndex parent(const QModelIndex &index) const override {
        if (!index.isValid())
            return QModelIndex();

        LeakTreeItem *childItem = getItem(index);
        LeakTreeItem *parentItem = childItem->parentItem();

        if (parentItem == &rootItem)
            return QModelIndex();

        return createIndex(parentItem->row(), 0, parentItem);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        LeakTreeItem *parentItem = getItem(parent);
        return parentItem->childCount();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 1; // Une seule colonne
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        LeakTreeItem *item = getItem(index);
        return item->data();
    }

private:
    LeakTreeItem* getItem(const QModelIndex &index) const {
        if (index.isValid()) {
            LeakTreeItem *item = static_cast<LeakTreeItem*>(index.internalPointer());
            if (item)
                return item;
        }
        return &rootItem;
    }

    class LeakTreeItem {
    public:
        explicit LeakTreeItem(const Leak &data, LeakTreeItem *parent = nullptr)
            : itemData(data), parentItem(parent) {}

        ~LeakTreeItem() {
            qDeleteAll(childItems);
        }

        void addChild(LeakTreeItem *child) {
            childItems.append(child);
        }

        LeakTreeItem *child(int row) {
            return childItems.value(row);
        }

        int childCount() const {
            return childItems.count();
        }

        int row() const {
            if (parentItem)
                return parentItem->childItems.indexOf(const_cast<LeakTreeItem*>(this));
            return 0;
        }

        Leak data() const {
            return itemData;
        }

        LeakTreeItem *parentItem;

    private:
        QList<LeakTreeItem*> childItems;
        Leak itemData;
    };

    LeakTreeItem rootItem;
};

#endif // LEAKMODEL_H
