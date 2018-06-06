#ifndef UDPSETTING_WIDGET_H
#define UDPSETTING_WIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class JIPAddressEdit;
class QSpinBox;
class QLineEdit;
class QCheckBox;
class QPushButton;

class UdpSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UdpSettingWidget(QWidget *parent = 0);
    ~UdpSettingWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    Icd::WorkerPtr d_worker;
    JIPAddressEdit *d_editLocalIP;
    QSpinBox *d_spinBoxLocalPort;
    JIPAddressEdit *d_editRemoteIP;
    QSpinBox *d_spinBoxRemotePort;
    QCheckBox *d_checkBoxReadOnly;
    QCheckBox *d_checkBoxWriteOnly;
    QLineEdit *d_editName;
    QLineEdit *d_editRelayer;
    QPushButton *d_buttonApply;
    QPushButton *d_buttonRestore;
};

#endif // UDPSETTING_WIDGET_H
