#ifndef EDITCREATEDLG_H
#define EDITCREATEDLG_H

#include <QDialog>

class QLineEdit;

namespace Edit {

// class EditCreateDlg

class EditCreateDlg : public QDialog
{
    Q_OBJECT
public:
    explicit EditCreateDlg(QWidget *parent = nullptr);

    QString filePath() const;

    void focusFilePath();

signals:

public slots:
    void setFilePath(const QString &filePath);

private:
    bool saveConfig();

private:
    QLineEdit *editFilePath_;
};

}

#endif // EDITCREATEDLG_H
