#ifndef ICDMAINVIEW_H
#define ICDMAINVIEW_H

#include <QWidget>
#include "../datamgr/datamanagement.h"

class ICDMainViewUi;

class ICDMainView : public QWidget
{
    Q_OBJECT
public:
    explicit ICDMainView(QWidget *parent = nullptr);
    virtual ~ICDMainView();

signals:

public slots:

protected:
    // 加载显示架构信息
    JLRESULT showInfrastructure(const Icd::JNEvent &event);
    // 查询主视图表数据信息
    JLRESULT queryTableInformation(const Icd::JNEvent &event);
    // 查询数据长度偏移
    JLRESULT queryLengthOffset(const Icd::JNEvent &event);
    // 查询当前表中已有数据（不包括当前行数据）
    JLRESULT queryExistedData(const Icd::JNEvent &event);
    // 查询当前表中的帧数据
    JLRESULT queryFrameData(const Icd::JNEvent &event);
    // 填充规则序号和字节序号
    JLRESULT fillBitSerial(const Icd::JNEvent &event);
    // 查询主视图控件状态
    JLRESULT queryMainViewState(const Icd::JNEvent &event);
    // 处理来自Navigation模块的命令
    JLRESULT dealNavigationCommand(const Icd::JNEvent &event);

private:
    ICDMainViewUi *q_ui;
};

#endif // ICDMAINVIEW_H
