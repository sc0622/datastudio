#ifndef DETAILTABLE_H
#define DETAILTABLE_H

#include <QWidget>

class JTableView;

namespace Edit {

class DetailTable : public QWidget
{
    Q_OBJECT
public:
    explicit DetailTable(QWidget *parent = nullptr);

signals:

public slots:

private:
    JTableView *d_tableView;
};

}

#endif // DETAILTABLE_H
