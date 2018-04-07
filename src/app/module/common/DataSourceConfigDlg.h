#ifndef DATASOURCECONFIGDLG_H
#define DATASOURCECONFIGDLG_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QTabWidget;
class QPushButton;

enum DataSourceType {
    DataSourceInvalid = -1,
    DataSourceFile,
    DataSourceSql
};

// class FileSourceWidget

class FileSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileSourceWidget(QWidget *parent = nullptr);

    QString filePath() const;

    void focusFilePath();

signals:
    void contentChanged();

public slots:
    void setFilePath(const QString &filePath);

private:
    QLineEdit *d_editFilePath;
};

// class SqlSourceWidget

class SqlSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SqlSourceWidget(QWidget *parent = nullptr);

    QString sqlType() const;
    QString serverName() const;
    QString databaseName() const;
    QString userName() const;
    QString password() const;

    void focusSqlType();
    void focusServerName();
    void focusDatabaseName();
    void focusUserName();
    void focusPassword();

signals:
    void contentChanged();

public slots:
    void setSqlType(const QString &sqlType);
    void setServrerName(const QString &serverName);
    void setDatabaseName(const QString &databaseName);
    void setUserName(const QString &userName);
    void setPassword(const QString &password);

private:
    QComboBox *d_comboBoxSqlType;
    QLineEdit *d_editServerName;
    QLineEdit *d_editDatabaseName;
    QLineEdit *d_editUserName;
    QLineEdit *d_editPassword;
};

// class DataSourceConfigDlg

class DataSourceConfigDlg : public QDialog
{
    Q_OBJECT
public:
    explicit DataSourceConfigDlg(const QString &module, QObject *receiver,
                                 QWidget *parent = nullptr);

signals:

public slots:

private:
    void updateTab(int index);
    bool saveConfig();
    bool restoreConfig();

private:
    QString d_module;
    QObject *d_receiver;
    QTabWidget *d_tabWidget;
    FileSourceWidget *d_fileSourceWidget;
    SqlSourceWidget *d_sqlSourceWidget;
    QPushButton *d_buttonConnect;
    QPushButton *d_buttonOk;
};

#endif // DATASOURCECONFIGDLG_H
