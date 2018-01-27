#ifndef SERIALSETTING_WIDGET_H
#define SERIALSETTING_WIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

//
class QComboBox;
class QLineEdit;
class QPushButton;

class SerialSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialSettingWidget(QWidget *parent = 0);
    ~SerialSettingWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    Icd::WorkerPtr d_worker;
    QComboBox *d_comboBoxPortName;
    QComboBox *d_comboBoxBaudRate;
    QComboBox *d_comboBoxDataBits;
    QComboBox *d_comboBoxStopBits;
    QComboBox *d_comboBoxParity;
    QLineEdit *d_editName;
    QLineEdit *d_editRelayer;
    QPushButton *d_buttonApply;
    QPushButton *d_buttonRestore;
};

#endif // SERIALSETTING_WIDGET_H
