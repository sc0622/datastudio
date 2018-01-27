#ifndef ICDMAINVIEW_H
#define ICDMAINVIEW_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ICDMainViewUi;

class ICDMainView
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public JObserver
{
public:
    explicit ICDMainView(IJAttempter* attemper);
    virtual ~ICDMainView();

    // IJUnknown interface
public:
    bool loadInterface();
    bool updateInterface();
    void releaseInterface();
    void *queryInterface(const std::string &iid, unsigned int ver);

    // IJComponent interface
public:
    std::string componentName() const;
    std::string componentDesc() const;
    void attach();
    void detach();

    // IJComponentUi interface
public:
    void *createWindow(void *parent, const std::string &objectName);

    // IJCommandSink interface
public:
    bool commandSink(void *sender, const std::string &domain,
                     const std::string &objectName,
                     const std::string &eventType, void *data);

    // JObserver interface
public:
    std::string observerId() const;

protected:
    // 加载显示架构信息
    JLRESULT showInfrastructure(const std::string &id,
                                JWPARAM wParam,
                                JLPARAM lParam);
    // 查询主视图表数据信息
    JLRESULT queryTableInformation(const std::string &id,
                                   JWPARAM wParam,
                                   JLPARAM lParam);
    // 查询数据长度偏移
    JLRESULT queryLengthOffset(const std::string &id,
                               JWPARAM wParam,
                               JLPARAM lParam);
    // 查询当前表中已有数据（不包括当前行数据）
    JLRESULT queryExistedData(const std::string &id,
                              JWPARAM wParam,
                              JLPARAM lParam);
    // 查询当前表中的帧数据
    JLRESULT queryFrameData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // 填充规则序号和字节序号
    JLRESULT fillBitSerial(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // 查询主视图控件状态
    JLRESULT queryMainViewState(const std::string &id,
                                JWPARAM wParam,
                                JLPARAM lParam);
    // 处理来自Navigation模块的命令
    JLRESULT dealNavigationCommand(const std::string &id,
                                   JWPARAM wParam,
                                   JLPARAM lParam);

private:
    IJAttempter *q_attempter;
    ICDMainViewUi *q_ui;
};

#endif // ICDMAINVIEW_H
