#ifndef SERIALSENDWIDGET_H
#define SERIALSENDWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QFormLayout;
class QCheckBox;
class QSpinBox;
class QPushButton;

class SerialSendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialSendWidget(QWidget *parent = 0);
    ~SerialSendWidget();

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

#endif // SERIALSENDWIDGET_H
