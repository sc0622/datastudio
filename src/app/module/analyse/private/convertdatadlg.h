#ifndef CONVERTDATADLG_H
#define CONVERTDATADLG_H

#include <QDialog>

class QTabWidget;
class ConvertFromIcdWidget;
class ParityDataDlg;
class ConvertToDataAnalyse;
class ConvertToIcdWidget;
class SplitFileDlg;

class ConvertDataDlg : public QDialog
{
public:
    explicit ConvertDataDlg(QWidget *parent = nullptr);
    ~ConvertDataDlg();

signals:

public slots:

private:
    QTabWidget *tabWidget_;
    ConvertFromIcdWidget *convFromIcdWidget_;
    ParityDataDlg *parityDataDlg_;
    ConvertToDataAnalyse *convtToDA_;
    //ConvertToIcdWidget *convToIcdWidget_;
    SplitFileDlg *splitFileDlg_;
};

#endif // CONVERTDATADLG_H
