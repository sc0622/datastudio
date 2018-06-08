#include "precomp.h"
#include "icdmainviewui.h"
#include "dataenginewidget.h"
#include "jwt/jsplitter.h"

ICDMainViewUi::ICDMainViewUi(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    q_dataWidget = new DataEngineWidget(this);
    horiLayoutMain->addWidget(q_dataWidget);
}

ICDMainViewUi::~ICDMainViewUi()
{

}

bool ICDMainViewUi::init()
{
    q_dataWidget->init();

    return true;
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
