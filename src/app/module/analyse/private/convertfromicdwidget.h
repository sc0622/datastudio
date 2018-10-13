#ifndef CONVERTFROMICDWIDGET_H
#define CONVERTFROMICDWIDGET_H

#include <QWidget>

class QLineEdit;
class QProgressBar;
class QPushButton;
class QCheckBox;
class QFile;

namespace Icd {
class Table;
typedef std::shared_ptr<Table> TablePtr;

} // end of namespace Icd

class ConvertFromIcdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConvertFromIcdWidget(QWidget *parent = nullptr);
    ~ConvertFromIcdWidget() override;

signals:
    void rejected();

public slots:

private:
    bool loadData(const QString &domain, int headerSize, bool hasTimeFormat);

private:
    QLineEdit *d_editSource;
    QLineEdit *d_editTarget;
    QCheckBox *d_checkParse;
    QProgressBar *d_progressBar;
    QPushButton *d_buttonConvert;
    QPushButton *d_buttonCancel;
    QString d_filePath;
    Icd::TablePtr d_table;
};

#endif // CONVERTFROMICDWIDGET_H
