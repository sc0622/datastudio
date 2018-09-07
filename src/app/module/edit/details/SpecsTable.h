#ifndef SPECSTABLE_H
#define SPECSTABLE_H

#include <QWidget>
#include <QStyledItemDelegate>

namespace Icd {
class Item;
typedef std::shared_ptr<Item> ItemPtr;
}

class JTableView;

namespace Edit {

// class SpecsTable

class SpecsTable : public QWidget
{
    Q_OBJECT
public:
    explicit SpecsTable(const Icd::ItemPtr &item, QWidget *parent = nullptr);

    Icd::ItemPtr item() const;

    bool init();

    void lock();
    void unlock();
    bool blocking() const;

    bool validate();
    void restoreContent();
    void saveContent();

signals:
    void contentChanged();

public slots:
    void onAdd();
    void onRemove();
    void onClean();

private:
    Icd::ItemPtr item_;
    JTableView *tableView_;
    bool blocking_;
};

// class JReadOnlyItemDelegate

class JReadOnlyItemDelegate : public QStyledItemDelegate
{
public:
    explicit JReadOnlyItemDelegate(const Icd::ItemPtr &item, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

private:
    Icd::ItemPtr item_;
};

// class JULongLongItemDelegate

class JULongLongItemDelegate : public QStyledItemDelegate
{
public:
    explicit JULongLongItemDelegate(const Icd::ItemPtr &item, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

private:
    Icd::ItemPtr item_;
};

// class JDoubleItemDelegate

class JDoubleItemDelegate : public QStyledItemDelegate
{
public:
    explicit JDoubleItemDelegate(const Icd::ItemPtr &item, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private:
    Icd::ItemPtr item_;
};

}

#endif // SPECSTABLE_H
