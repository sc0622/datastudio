#ifndef FILEDETAILWIDGET_H
#define FILEDETAILWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class FileSettingWidget;
class FileSendWidget;
class FileRecvWidget;

class FileDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileDetailWidget(QWidget *parent = nullptr);
    ~FileDetailWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:
    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();

private:
    Icd::WorkerPtr d_worker;
    FileSettingWidget *d_settingWidget;
    FileSendWidget *d_sendWidget;
    FileRecvWidget *d_recvWidget;
};

#endif // FILEDETAILWIDGET_H
