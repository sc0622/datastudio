#ifndef ICDLISTWIDGET_H
#define ICDLISTWIDGET_H

#include <QWidget>
#include "icdcore/icdcore_global.h"

namespace Icd {
class Object;
typedef JHandlePtr<Object> ObjectPtr;
class Table;
typedef JHandlePtr<Table> TablePtr;
class Item;
typedef JHandlePtr<Item> ItemPtr;
class FrameItem;
typedef JHandlePtr<FrameItem> FrameItemPtr;
class Object;
typedef JHandlePtr<Object> ObjectPtr;

}

// struct ItemInfo

struct ItemInfo
{
    Icd::ItemPtr item;
    int bitOffset;

    ItemInfo()
        : item(Icd::ItemPtr(nullptr))
        , bitOffset(-1)
    {

    }

    explicit ItemInfo(const Icd::ItemPtr &item, int bitOffset = -1)
        : item(item)
        , bitOffset(bitOffset)
    {

    }

    ItemInfo(const ItemInfo &other)
        : item(other.item)
        , bitOffset(other.bitOffset)
    {

    }

    ItemInfo &operator =(const ItemInfo &other)
    {
        if (this == &other) {
            return *this;
        }
        item = other.item;
        bitOffset = other.bitOffset;
        return *this;
    }

    bool isNull() const
    {
        return (!item);
    }
};

// class IcdListWidget

class QListWidget;

class IcdListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IcdListWidget(const Icd::TablePtr &table, const Icd::ObjectPtr &first,
                           const QString &filePath, int bitOffset = -1,
                           QWidget *parent = nullptr);

    QList<ItemInfo> items();
    const QList<ItemInfo> &items() const;

    static QString generateItemNamePath(const Icd::ItemPtr &item, int bitOffset = -1);

signals:

public slots:

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    //
    void onDragEnterEvent(QDragEnterEvent *event);
    void onDropEvent(QDropEvent *event);

    //
    bool addItem(const Icd::ItemPtr &item, int bitOffset = -1);
    bool addItem(const Icd::TablePtr &table, int bitOffset = -1);
    bool addItem(const Icd::FrameItemPtr &frame, int bitOffset = -1);
    bool addItem(const Icd::ObjectPtr &object, int bitOffset = -1);
    void removeSelectedItems();

private:
    QListWidget *d_listWidget;
    Icd::TablePtr d_table;
    QString d_filePath;
    QList<ItemInfo> d_items;
};

#endif // ICDLISTWIDGET_H
