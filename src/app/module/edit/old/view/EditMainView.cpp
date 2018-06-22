#include "precomp.h"
#include "EditMainView.h"
#include "DataEditWidget.h"
#include "jwt/jsplitter.h"

EditMainView::EditMainView(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* horiLayoutMain = new QHBoxLayout(this);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    q_dataWidget = new DataEditWidget(this);
    horiLayoutMain->addWidget(q_dataWidget);

    jnotify->on("edit.toolbar.window.settings", this, [=](JNEvent &event){
        setVisible(event.argument().toBool());
    });
    jnotify->on("edit.showInfrastructure", this, [=](Icd::JNEvent &event){
        showInfrastructure(event);
    });
    jnotify->on("edit.queryTableInformation", this, [=](Icd::JNEvent &event){
        queryTableInformation(event);
    });
    jnotify->on("edit.queryLengthOffset", this, [=](Icd::JNEvent &event){
        queryLengthOffset(event);
    });
    jnotify->on("edit.queryExistedData", this, [=](Icd::JNEvent &event){
        queryExistedData(event);
    });
    jnotify->on("edit.queryFrameData", this, [=](Icd::JNEvent &event){
        queryFrameData(event);
    });
    jnotify->on("edit.fillBitSerial", this, [=](Icd::JNEvent &event){
        fillBitSerial(event);
    });
    jnotify->on("edit.queryMainViewState", this, [=](Icd::JNEvent &event){
        queryMainViewState(event);
    });
    jnotify->on("edit.dealNavigationCommand", this, [=](Icd::JNEvent &event){
        dealNavigationCommand(event);
    });
}

EditMainView::~EditMainView()
{

}

bool EditMainView::init()
{
    q_dataWidget->init();

    return true;
}

/**
 * @brief 初始化数据
 * @param [in] type : 数据类型(GLOBALDEF::NODETYPE)
 * @param [in] data : 数据
 */
void EditMainView::initData(int type, void *data)
{
    q_dataWidget->initUI(type, data);
}

int EditMainView::queryTableInformation(const QString &type) const
{
    return q_dataWidget->queryTableInformation(type);
}

int EditMainView::queryLengthOffset(const ICDMetaData::smtMeta &meta) const
{
    return q_dataWidget->queryOffset(meta);
}

QMap<QString, QString> EditMainView::queryExistedData(const QString &section) const
{
    return q_dataWidget->queryExistedData(section);
}

QMap<int, QString> EditMainView::queryFrameData(const QString &type) const
{
    return q_dataWidget->queryFrameData(type);
}

void EditMainView::fillBitSerial(ICDMetaData::smtMeta &meta)
{
    q_dataWidget->fillBitIndex(meta);
}

bool EditMainView::queryWidgetState(const QString &name) const
{
    return q_dataWidget->queryWidgetState(name);
}

void EditMainView::dealCommand(int command, const QVariant &param)
{
    q_dataWidget->dealCommand(command, param);
}

// 加载显示架构信息
JLRESULT EditMainView::showInfrastructure(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    void *data = jVariantFromVoid<void>(args[0]);
    if (!data) {
        return -1;
    }

    int type = args[1].toInt();

    initData(type, data);

    return 0;
}

// 查询主视图表数据信息
JLRESULT EditMainView::queryTableInformation(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    //
    int *result = jVariantFromVoid<int>(args[0]);
    if (!result) {
        return -1;
    }

    QString *type = jVariantFromVoid<QString>(args[1]);
    if (!type) {
        return -1;
    }

    *result = queryTableInformation(*type);

    return 0;
}

// 查询数据长度偏移
JLRESULT EditMainView::queryLengthOffset(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    ICDMetaData::smtMeta *meta = jVariantFromVoid<ICDMetaData::smtMeta>(args[0]);
    if (!meta) {
        return -1;
    }

    int *result = jVariantFromVoid<int>(args[1]);
    if (!result) {
        return -1;
    }

    *result = queryLengthOffset(*meta);

    return 0;
}

// 查询当前表中已有数据（不包括当前行数据）
JLRESULT EditMainView::queryExistedData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    QMap<QString, QString> *result = jVariantFromVoid<QMap<QString, QString> >(args[0]);
    if (!result) {
        return -1;
    }

    const QString *section = jVariantFromVoid<QString>(args[1]);
    if (!section) {
        return -1;
    }

    *result = queryExistedData(*section);

    return 0;
}

// 查询当前表中的帧数据
JLRESULT EditMainView::queryFrameData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    QMap<int, QString> *frame = jVariantFromVoid<QMap<int, QString> >(args[0]);
    if (!frame) {
        return -1;
    }

    QString *type = jVariantFromVoid<QString>(args[1]);
    if (!type) {
        return -1;
    }

    *frame = queryFrameData(*type);

    return 0;
}

// 填充规则序号和字节序号
JLRESULT EditMainView::fillBitSerial(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }

    ICDMetaData::smtMeta *meta = jVariantFromVoid<ICDMetaData::smtMeta>(args[0]);
    if (!meta) {
        return -1;
    }

    fillBitSerial(*meta);

    return 0;
}

// 查询主视图控件状态
JLRESULT EditMainView::queryMainViewState(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    const QString *name = jVariantFromVoid<QString>(args[0]);
    if (!name) {
        return -1;
    }

    bool *result = jVariantFromVoid<bool>(args[1]);
    if (!result) {
        return -1;
    }

    *result = queryWidgetState(*name);

    return 0;
}

// 处理来自Navigation模块的命令
JLRESULT EditMainView::dealNavigationCommand(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    const QVariant *var = jVariantFromVoid<QVariant>(args[1]);
    if (!var) {
        return -1;
    }

    dealCommand(args[0].toInt(), *var);

    return 0;
}
