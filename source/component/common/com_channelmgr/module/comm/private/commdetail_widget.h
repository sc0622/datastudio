#ifndef COMMDETAIL_WIDGET_H
#define COMMDETAIL_WIDGET_H

#include <QFrame>
#include "icdworker/icdworker.h"

class QStackedWidget;
class SerialDetailWidget;
class UdpDetailWidget;
class FileDetailWidget;

class CommDetailWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CommDetailWidget(QWidget *parent = 0);

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    QStackedWidget *d_stackedWidget;
    SerialDetailWidget *d_serialDetailWidget;
    UdpDetailWidget *d_udpDetailWidget;
    FileDetailWidget *d_fileDetailWidget;
};

#endif // COMMDETAIL_WIDGET_H
