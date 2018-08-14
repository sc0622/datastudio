#ifndef FILERECVWIDGET_H
#define FILERECVWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QSpinBox;
class QPushButton;
class QLineEdit;

class FileRecvWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileRecvWidget(QWidget *parent = nullptr);
    ~FileRecvWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    Icd::WorkerPtr d_worker;
    QSpinBox *d_spinBoxInterval;
    QPushButton *d_buttonApply;
    QPushButton *d_buttonRestore;
    QLineEdit *d_editSequence;
};

#endif // FILERECVWIDGET_H
