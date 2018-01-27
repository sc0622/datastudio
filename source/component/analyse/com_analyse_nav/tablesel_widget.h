#ifndef TABLESEL_WIDGET_H
#define TABLESEL_WIDGET_H

#include <QDialog>

namespace Icd {

class CoreTreeWidget;

}

class QStandardItem;

class TableSelWidget : public QDialog
{
    Q_OBJECT
public:
    explicit TableSelWidget(QWidget *parent = 0);
    ~TableSelWidget();

    bool init();

    /**
     * @brief tableDomain
     * @return
     */
    QString tableDomain() const;

Q_SIGNALS:

public Q_SLOTS:
    void accept();

private:
    Icd::CoreTreeWidget *d_icdTree;
    QStandardItem *d_currentItem;
};

#endif // TABLESEL_WIDGET_H
