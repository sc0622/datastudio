#ifndef MAINSETTINGSDLG_H
#define MAINSETTINGSDLG_H

#include <QDialog>

namespace Main {

class SettingsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDlg(QWidget *parent = nullptr);
    ~SettingsDlg();

signals:

public slots:

private:
};

}

#endif // MAINSETTINGSDLG_H
