#ifndef DETAILITEMEDIT_H
#define DETAILITEMEDIT_H

#include <QWidget>

class QGroupBox;

namespace Edit {

class DetailItemEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DetailItemEdit(QWidget *parent = nullptr);

signals:

public slots:

private:
    QGroupBox *d_groupBox;
};

}

#endif // DETAILITEMEDIT_H
