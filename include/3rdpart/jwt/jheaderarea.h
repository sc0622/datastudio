#ifndef JHEADERAREA_H
#define JHEADERAREA_H

#include "jwt_global.h"
#include <QSortFilterProxyModel>
#include <QAbstractItemView>

// - class JHeaderArea -

class JHeaderAreaPrivate;
class JTableSortFilterModel;

class JWT_EXPORT JHeaderArea : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int titleHeight READ titleHeight WRITE setTitleHeight NOTIFY titleHeightChanged)
    Q_PROPERTY(int filterHeight READ filterHeight WRITE setFilterHeight NOTIFY filterHeightChanged)
public:
    explicit JHeaderArea(QWidget *parent = 0);
    ~JHeaderArea();

    bool autoUpdateTitle() const;
    bool titleVisible() const;
    QString title() const;
    int titleHeight() const;
    QFont titleFont() const;
    Qt::Alignment titleAlignment() const;
    bool filterVisible() const;
    int filterHeight() const;

    //! QLabel - stypeSheet
    void setTitleStyle(const QString &styleSheet);
    //! QWidget - styleSheet
    void setFilterStyle(const QString &styleSheet);

    //! filter
    bool filterItemVisible(int column) const;
    QStringList filterItem(int column) const;
    bool filterItemEditable(int column) const;
    void setFilterItemEditable(bool editable, int column = -1);
    bool setFilterItem(int column, const QString &text);    // QLineEdit
    bool setFilterItem(int column, const QStringList &texts = QStringList());   // QComboBox
    void setFilterItem(const QList<int> &columns);  // QLineEdit
    void setFilterItem(const QList<QPair<int, QString> > &columns); // QLineEdit
    void setFilterItem(const QList<QPair<int, QStringList> > &columns); // QComboBox
    void setFilterItemAsComboBox(const QList<int> columns); // QComboBox
    void setAllFilterItemWithLineEdit();    // QLineEdit
    void setAllFilterItemWithComboBox();    // QComboBox
    void removeFilterItem(int column);
    void clearFilterItem();

    JTableSortFilterModel *filterModel();
    void setFilterModel(JTableSortFilterModel *model);

    bool attach(QAbstractItemView *view);
    void detach();

Q_SIGNALS:
    void titleVisibleChanged(bool);
    void titleChanged(const QString &);
    void titleHeightChanged(int);
    void filterHeightChanged(int);
    void filterVisibleChanged(bool);
    void attached();
    void detached();

public Q_SLOTS:
    void setAutoUpdateTitle(bool enable = true);
    void setTitleVisible(bool visible = true);
    void setTitle(const QString &text);
    void setTitleHeight(int height);
    void setTitleFont(const QFont &font);
    void setTitleAlignment(Qt::Alignment alignment);
    void setFilterVisible(bool visible = true);
    void setFilterHeight(int height);
    void setSwitchEnabled(bool enable = true);

private:
    Q_DISABLE_COPY(JHeaderArea)
    J_DECLARE_PRIVATE(JHeaderArea)
};

#endif // JHEADERAREA_H
