#ifndef PARITYDATADLG_H
#define PARITYDATADLG_H

#include <QWidget>

class QLineEdit;
class QProgressBar;
class QPushButton;
class QCheckBox;

namespace Icd {

class Table;
typedef std::shared_ptr<Table> TablePtr;

} // end of namespace Icd

class ParityDataDlg : public QWidget
{
    Q_OBJECT
public:
    explicit ParityDataDlg(QWidget *parent = Q_NULLPTR);
    ~ParityDataDlg();

signals:
    void rejected();

public slots:

private:
    bool loadData(const QString &domain, int headerSize,
                  bool hasTimeFormat);

private:
    QLineEdit *d_editSource;
    QLineEdit *d_editTarget;
    QProgressBar *d_progressBar;
    QPushButton *d_buttonConvert;
    QPushButton *d_buttonCancel;
    Icd::TablePtr d_table;
};

#endif // PARITYDATADLG_H
