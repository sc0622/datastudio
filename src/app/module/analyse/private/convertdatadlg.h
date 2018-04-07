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
    explicit ConvertDataDlg(QWidget *parent = 0);
    ~ConvertDataDlg();

signals:

public slots:

private:
    QTabWidget *d_tabWidget;
    ConvertFromIcdWidget *d_convFromIcdWidget;
    ParityDataDlg *d_parityDataDlg;
    ConvertToDataAnalyse *d_convtToDA;
    //ConvertToIcdWidget *d_convToIcdWidget;
    SplitFileDlg *d_splitFileDlg;
};

#endif // CONVERTDATADLG_H
