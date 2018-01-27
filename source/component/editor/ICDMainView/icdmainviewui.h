#ifndef ICDMAINVIEWUI_H
#define ICDMAINVIEWUI_H

#include <QWidget>
#include "planenode.h"

class IJAttempter;
class DataEngineWidget;

class ICDMainViewUi : public QWidget
{
    Q_OBJECT
public:
    explicit ICDMainViewUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ICDMainViewUi();

    /**
     * @brief 初始化数据
     * @param [in] type : 数据类型(GLOBALDEF::NODETYPE)
     * @param [in] data : 数据
     */
    void initData(int type, void* data);

    // 查询当前显示表数据信息
    int queryTableInformation(const QString &type) const;

    // 查询数据长度偏移量
    int queryLengthOffset(const ICDMetaData::smtMeta &meta) const;

    // 查询已存在表中的数据
    QMap<QString, QString> queryExistedData(const QString &section) const;

    // 查询帧数据
    QMap<int, QString> queryFrameData(const QString &type) const;

    // 填充bit类型数据的起始字节号和序号
    void fillBitSerial(ICDMetaData::smtMeta &meta);

    // 查询界面控件状态
    bool queryWidgetState(const QString &name) const;

    // 处理来自其他模块的命令
    void dealCommand(int command, const QVariant &param);

    //
    void setMenuPtr(const std::string &name, QAction *acttion);

    //
    bool dealMenuCmd(const std::string &name);

Q_SIGNALS:

public Q_SLOTS:

private:
    IJAttempter &q_attempter;

    DataEngineWidget    *q_dataWidget;      // 信息编辑窗口
};

#endif // ICDMAINVIEWUI_H
