#ifndef JFtpTableModel_H
#define JFtpTableModel_H

#include "../../icdmeta_global.h"
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

namespace icdmeta {

// FtpTableSortModel

class ICDMETA_EXPORT JFtpTableSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit JFtpTableSortModel(QObject *parent = Q_NULLPTR);
    ~JFtpTableSortModel();

    // QSortFilterProxyModel interface
protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
};

// class JFtpTableModel

class JFtpTableModelPrivate;

class ICDMETA_EXPORT JFtpTableModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit JFtpTableModel(QObject *parent = 0);
    ~JFtpTableModel();

    enum UserRole {
        NameRole = Qt::UserRole + 1,
        PermissionsRole,
        SizeRole,
        LastModifiedRole,
        LastReadRole,
        OwnerRole,
        GroupRole,
        IsDirRole,
        IsFileRole,
        IsSymLinkRole,
        IsWritableRole,
        IsReadableRole,
        IsExcutableRole,
        TotalCount = IsExcutableRole - NameRole + 1
    };
    Q_ENUM(UserRole)

    static void registerQmlType();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    //
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariant data(int row, int role = Qt::DisplayRole);
    Q_INVOKABLE bool setData(int row, const QVariant &value, int role = Qt::EditRole);
    Q_INVOKABLE void clearContents();

    Q_INVOKABLE void setSort(bool isRootDir);

signals:

public slots:

private:
    Q_DISABLE_COPY(JFtpTableModel)
    J_DECLARE_PRIVATE(JFtpTableModel)
};

}

#endif // JFtpTableModel_H
