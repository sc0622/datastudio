#ifndef FILESETTING_WIDGET_H
#define FILESETTING_WIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QLineEdit;
class QCheckBox;
class QPushButton;

class FileSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileSettingWidget(QWidget *parent = nullptr);
    ~FileSettingWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    Icd::WorkerPtr d_worker;
    QLineEdit *d_editFilePath;
    QCheckBox *d_checkBoxReadOnly;
    QCheckBox *d_checkBoxWriteOnly;
    QCheckBox *d_checkBoxAppend;
    QCheckBox *d_checkBoxTruncate;
    QCheckBox *d_checkBoxText;
    QLineEdit *d_editName;
    QCheckBox *d_checkBoxDomain;
    QCheckBox *d_checkBoxTimestamp;
    QLineEdit *d_editRelayer;
    QPushButton *d_buttonApply;
    QPushButton *d_buttonRestore;
};

#endif // FILESETTING_WIDGET_H
