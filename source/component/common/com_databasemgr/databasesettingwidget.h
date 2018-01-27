#ifndef DATABASESETTINGWIDGET_H
#define DATABASESETTINGWIDGET_H

#include <QtWidgets>

class IJAttempter;

class DatabaseSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatabaseSettingWidget(IJAttempter &attempter,
                                   QWidget *parent = 0);

    // ��ʼ����������
    bool initUi();
    //
    bool dataValid();

signals:
    void configChanged();

public Q_SLOTS:
    void slotApplyButtonClicked();
    void slotDriverChanged(const QString &text);

protected:
    // �������ݿ���Ϣ
    bool updateDatabase();
    //
    void enableButtons(bool enabled);

private:
    IJAttempter     &d_attempter;
    QComboBox       *d_driver;  // ��������
    QLineEdit       *d_server;  // ������
    QLabel          *d_dbLabel; //
    QLineEdit       *d_database;    // ���ݿ�����Sql server��
    QLineEdit       *d_user;    // �û���
    QLineEdit       *d_password;    // ����
    QPushButton     *d_buttonApply; // Ӧ��
    QPushButton     *d_buttonCancel;// ȡ��
    bool            d_valid;    // ������Ч��־
};

#endif // DATABASESETTINGWIDGET_H
