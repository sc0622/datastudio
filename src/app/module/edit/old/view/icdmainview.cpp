#include "precomp.h"
#include "icdmainview.h"
#include "icdmainviewui.h"
#include "KernelClass/planenode.h"

ICDMainView::ICDMainView(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layoutMain = new QHBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    q_ui = new ICDMainViewUi(this);
    layoutMain->addWidget(q_ui);

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

ICDMainView::~ICDMainView()
{

}

// ������ʾ�ܹ���Ϣ
JLRESULT ICDMainView::showInfrastructure(const Icd::JNEvent &event)
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

    q_ui->initData(type, data);

    return 0;
}

// ��ѯ����ͼ��������Ϣ
JLRESULT ICDMainView::queryTableInformation(const Icd::JNEvent &event)
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

    *result = q_ui->queryTableInformation(*type);

    return 0;
}

// ��ѯ���ݳ���ƫ��
JLRESULT ICDMainView::queryLengthOffset(const Icd::JNEvent &event)
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

    *result = q_ui->queryLengthOffset(*meta);

    return 0;
}

// ��ѯ��ǰ�����������ݣ���������ǰ�����ݣ�
JLRESULT ICDMainView::queryExistedData(const Icd::JNEvent &event)
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

    *result = q_ui->queryExistedData(*section);

    return 0;
}

// ��ѯ��ǰ���е�֡����
JLRESULT ICDMainView::queryFrameData(const Icd::JNEvent &event)
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

    *frame = q_ui->queryFrameData(*type);

    return 0;
}

// ��������ź��ֽ����
JLRESULT ICDMainView::fillBitSerial(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }

    ICDMetaData::smtMeta *meta = jVariantFromVoid<ICDMetaData::smtMeta>(args[0]);
    if (!meta) {
        return -1;
    }

    q_ui->fillBitSerial(*meta);

    return 0;
}

// ��ѯ����ͼ�ؼ�״̬
JLRESULT ICDMainView::queryMainViewState(const Icd::JNEvent &event)
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

    *result = q_ui->queryWidgetState(*name);

    return 0;
}

// ��������Navigationģ�������
JLRESULT ICDMainView::dealNavigationCommand(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    const QVariant *var = jVariantFromVoid<QVariant>(args[1]);
    if (!var) {
        return -1;
    }

    q_ui->dealCommand(args[0].toInt(), var);

    return 0;
}
