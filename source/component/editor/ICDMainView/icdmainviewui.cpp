#include "icdmainviewui.h"
#include "dataenginewidget.h"
#include "jsplitter.h"

ICDMainViewUi::ICDMainViewUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , q_attempter(attempter)
{
    q_dataWidget = new DataEngineWidget(attempter, this);

    QHBoxLayout* horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);
    horiLayoutMain->addWidget(q_dataWidget);
}

ICDMainViewUi::~ICDMainViewUi()
{

}

/**
 * @brief 初始化数据
 * @param [in] type : 数据类型(GLOBALDEF::NODETYPE)
 * @param [in] data : 数据
 */
void ICDMainViewUi::initData(int type, void *data)
{
    q_dataWidget->initUI(type, data);
}

int ICDMainViewUi::queryTableInformation(const QString &type) const
{
    return q_dataWidget->queryTableInformation(type);
}

int ICDMainViewUi::queryLengthOffset(const ICDMetaData::smtMeta &meta) const
{
    return q_dataWidget->queryOffset(meta);
}

QMap<QString, QString> ICDMainViewUi::queryExistedData(const QString &section) const
{
    return q_dataWidget->queryExistedData(section);
}

QMap<int, QString> ICDMainViewUi::queryFrameData(const QString &type) const
{
    return q_dataWidget->queryFrameData(type);
}

void ICDMainViewUi::fillBitSerial(ICDMetaData::smtMeta &meta)
{
    q_dataWidget->fillBitIndex(meta);
}

bool ICDMainViewUi::queryWidgetState(const QString &name) const
{
    return q_dataWidget->queryWidgetState(name);
}

void ICDMainViewUi::dealCommand(int command, const QVariant &param)
{
    q_dataWidget->dealCommand(command, param);
}

void ICDMainViewUi::setMenuPtr(const std::string &name, QAction *acttion)
{
    q_dataWidget->setMenuPtr(name, acttion);
}

bool ICDMainViewUi::dealMenuCmd(const std::string &name)
{
    return q_dataWidget->dealMenuCmd(name);
}
