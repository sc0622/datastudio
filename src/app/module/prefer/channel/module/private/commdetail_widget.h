#ifndef COMMDETAIL_WIDGET_H
#define COMMDETAIL_WIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QScrollArea;
class QStackedWidget;
class SerialDetailWidget;
class UdpDetailWidget;
class FileDetailWidget;

class CommDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommDetailWidget(QWidget *parent = 0);

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    QScrollArea *d_scrollArea;
    QStackedWidget *d_stackedWidget;
    SerialDetailWidget *d_serialDetailWidget;
    UdpDetailWidget *d_udpDetailWidget;
    FileDetailWidget *d_fileDetailWidget;
};

#endif // COMMDETAIL_WIDGET_H
