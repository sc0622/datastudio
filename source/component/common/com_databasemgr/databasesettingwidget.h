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

    // 初始化界面数据
    bool initUi();
    //
    bool dataValid();

signals:
    void configChanged();

public Q_SLOTS:
    void slotApplyButtonClicked();
    void slotDriverChanged(const QString &text);

protected:
    // 更新数据库信息
    bool updateDatabase();
    //
    void enableButtons(bool enabled);

private:
    IJAttempter     &d_attempter;
    QComboBox       *d_driver;  // 驱动类型
    QLineEdit       *d_server;  // 服务名
    QLabel          *d_dbLabel; //
    QLineEdit       *d_database;    // 数据库名（Sql server）
    QLineEdit       *d_user;    // 用户名
    QLineEdit       *d_password;    // 密码
    QPushButton     *d_buttonApply; // 应用
    QPushButton     *d_buttonCancel;// 取消
    bool            d_valid;    // 数据有效标志
};

#endif // DATABASESETTINGWIDGET_H
