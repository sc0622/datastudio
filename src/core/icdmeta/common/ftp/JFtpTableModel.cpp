#include "precomp.h"
#include "JFtpTableModel.h"

namespace icdmeta {

// JFtpTableSortModel

JFtpTableSortModel::JFtpTableSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

JFtpTableSortModel::~JFtpTableSortModel()
{

}

bool JFtpTableSortModel::lessThan(const QModelIndex &source_left,
                                 const QModelIndex &source_right) const
{
    return true;
    if (!source_left.isValid() || !source_right.isValid()) {
        return false;
    }
    if (source_left.column() == 0 && source_right.column() == 0) {
        const bool left_isDir = source_left.data(JFtpTableModel::IsDirRole).toBool();
        const bool right_isDir = source_right.data(JFtpTableModel::IsDirRole).toBool();
        if (right_isDir) {
            if (!left_isDir) {
                return true;
            }
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

// class JFtpTableModelPrivate

class JFtpTableModelPrivate
{
public:
    JFtpTableModelPrivate(JFtpTableModel *q)
        : J_QPTR(q)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JFtpTableModel)
    QList<QMap<int, QVariant> > items;
};

void JFtpTableModelPrivate::init()
{

}

// class FtpModel

JFtpTableModel::JFtpTableModel(QObject *parent)
    : QAbstractListModel(parent)
    , J_DPTR(new JFtpTableModelPrivate(this))
{
    Q_D(JFtpTableModel);
    d->init();
}

JFtpTableModel::~JFtpTableModel()
{
    Q_D(JFtpTableModel);
    delete d;
}

void JFtpTableModel::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JFtpTableModel);

    //
}

int JFtpTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    Q_D(const JFtpTableModel);
    return d->items.count();
}

QVariant JFtpTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Q_D(const JFtpTableModel);
    if (index.row() < 0 || index.row() >= d->items.count()) {
        return QVariant();
    }
    const auto &item = d->items[index.row()];
    const auto citer = item.constFind(role);
    if (citer == item.cend()) {
        return QVariant("");
    }
    return citer.value();
}

bool JFtpTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(JFtpTableModel);
    auto &item = d->items[index.row()];
    const auto citer = item.constFind(role);
    if (citer == item.cend() || citer.value() != value) {
        item[role] = value;
        emit dataChanged(index, index, QVector<int>() << role);
    }
    return false;
}

Qt::ItemFlags JFtpTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool JFtpTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    Q_D(JFtpTableModel);
    while (--count >= 0) {
        d->items.insert(row, QMap<int, QVariant>());
    }
    endInsertRows();

    return true;
}

bool JFtpTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_D(JFtpTableModel);
    if (0 <= row && row < d->items.count()) {
        beginRemoveRows(parent, row, row + count - 1);
        count = qMin(count, d->items.count() - row);
        while (--count >= 0) {
            d->items.removeAt(row);
        }
        endRemoveRows();
    }

    return false;
}

QHash<int, QByteArray> JFtpTableModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(NameRole, "name");
    names.insert(PermissionsRole, "permissions");
    names.insert(SizeRole, "size");
    names.insert(LastModifiedRole, "lastModified");
    names.insert(LastReadRole, "lastRead");
    names.insert(OwnerRole, "owner");
    names.insert(GroupRole, "group");
    names.insert(IsDirRole, "isDir");
    names.insert(IsFileRole, "isFile");
    names.insert(IsSymLinkRole, "isSymLink");
    names.insert(IsWritableRole, "isWritable");
    names.insert(IsReadableRole, "isReadable");
    names.insert(IsExcutableRole, "isExecutable");
    return names;
}

QVariant JFtpTableModel::data(int row, int role)
{
    return data(createIndex(row, 0), role);
}

bool JFtpTableModel::setData(int row, const QVariant &value, int role)
{
    return setData(createIndex(row, 0), value, role);
}

void JFtpTableModel::clearContents()
{
    Q_D(JFtpTableModel);
    removeRows(0, d->items.count());
}

void JFtpTableModel::setSort(bool isRootDir)
{
    Q_D(JFtpTableModel);
    qSort(d->items.begin() + (isRootDir ? 0 : 1), d->items.end(),
          [=](const QMap<int, QVariant> &first, const QMap<int, QVariant> &second){
#if 0   // folders > files
        auto lessThan = [=]() -> bool {
            const QString firstName = first[JFtpTableModel::NameRole].toString();
            const QString secondName = second[JFtpTableModel::NameRole].toString();
            return firstName < secondName;
        };

        const bool firstIsDir = first[JFtpTableModel::IsDirRole].toBool();
        const bool secondIsDir = second[JFtpTableModel::IsDirRole].toBool();
        if (secondIsDir) {
            if (firstIsDir) {
                return lessThan();
            } else {
                return false;
            }
        } else {
            if (!firstIsDir) {
                return lessThan();
            }
        }
#else   // sort by modified
        const QDateTime firstDateTime = first[JFtpTableModel::LastModifiedRole].toDateTime();
        const QDateTime secondDateTime = second[JFtpTableModel::LastModifiedRole].toDateTime();
        if (firstDateTime < secondDateTime) {
            return false;
        }
#endif
        return true;
    });

    emit dataChanged(createIndex(0, 0),
                     createIndex(d->items.count() - 1, TotalCount - 1));
}

}
