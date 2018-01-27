#ifndef FILESENDWIDGET_H
#define FILESENDWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QFormLayout;
class QCheckBox;
class QSpinBox;
class QPushButton;

class FileSendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileSendWidget(QWidget *parent = 0);
    ~FileSendWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:
    void onWorkerClosed();
    void onWorkerStarted();
    void onWorkerStopped();
    void onButtonSwitchClicked();

private:
    Icd::WorkerPtr d_worker;
    QFormLayout *d_formLayout;
    QCheckBox *d_checkBoxPeriodic;
    QSpinBox *d_spinBoxInterval;
    QPushButton *d_buttonSwitch;
    QPushButton *d_buttonApply;
    QPushButton *d_buttonRestore;
};

#endif // FILESENDWIDGET_H
