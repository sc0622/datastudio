#ifndef LOGGINGWIDGET_H
#define LOGGINGWIDGET_H

#include "ObjectEdit.h"

class QGroupBox;
class QComboBox;
class QStackedWidget;

class LoggingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoggingWidget(QWidget *parent = nullptr);

    // 初始化界面数据
    void initUIData(int windowType, const _UIData &data);
    // 获取界面数据
    void *uiData();
    // 启/停用界面控制按钮
    void enableOptionButton(bool enable);

signals:
    void dataSaved(void *data, bool &result);
    void canceled();

public slots:
    // 保存数据
    void slotConfirm(bool &result);
    // 取消
    void slotCancel();
    // 数据类型变更
    void slotTypeChanged(int index);

private:
    // 初始化数据类型信息
    void initTypeInfo(const _UIData &data);
    // 启/停用信号槽
    void enableConnection(bool enable);

private:
    QGroupBox *groupType_;
    QComboBox *boxType_;
    QStackedWidget *stackedWidget_;
    ObjectEdit *currentEdit_;
};

#endif // LOGGINGWIDGET_H
