#ifndef LOGGINGWIDGET_H
#define LOGGINGWIDGET_H

#include <QtWidgets>

struct _UIData;
class MetaUI;

class LoggingWidget
        : public QWidget
{
    Q_OBJECT
public:
    enum WIDGEGTYPE {
        wdUnknown = -1,
        wdHeader,   // 表头界面
        wdCounter,  // 帧计数界面
        wdCheck,    // 校验界面
        wdFrameCode,// 帧识别码
        wdCommon,   // 通用界面
        wdBitMap,   // bitMap界面
        wdBitValue, // bitValue界面
        wdDiscern,  // 帧数据界面
        wdComplex,  // 复合数据界面
        wdBuffer,   // 数据预留区界面
        wdSubTable, // 子表界面
        wdTable,    // 表界面
        wdSystem,   // 系统界面
        wdPlane,    // 机型界面
    };

public:
    explicit LoggingWidget(QWidget *parent = nullptr);
    // 设置当前显示窗口
    void setCurrentWidget(WIDGEGTYPE type);
    // 初始化界面数据
    void initUIData(WIDGEGTYPE type, const _UIData &data);
    // 获取界面数据
    void *uiData();
    // 启/停用界面控制按钮
    void enableOptionButton(bool enable);

signals:
    void dataSaved(void *data, bool &result);
    void canceled();

protected slots:
    // 保存数据
    void slotConfirm(bool &result);
    // 取消
    void slotCancel();
    // 数据类型变更
    void slotTypeChanged(int index);

protected:
    // 初始化数据类型信息
    void initTypeInfo(const _UIData &data);
    // 启/停用信号槽
    void enableConnection(bool enable);

private:
    QGroupBox                   *q_groupType;
    QComboBox                   *q_boxType;
    QStackedWidget              *q_stack;
    QHash<WIDGEGTYPE, MetaUI*>  q_widget;
    QHash<int, WIDGEGTYPE>      q_relation;
};

#endif // LOGGINGWIDGET_H
