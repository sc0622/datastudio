#ifndef EDITMAINVIEW_H
#define EDITMAINVIEW_H

#include <QWidget>
#include "KernelClass/planenode.h"
#include "../datamgr/datamanagement.h"

class DataEditWidget;

class EditMainView : public QWidget
{
    Q_OBJECT
public:
    explicit EditMainView(QWidget *parent = nullptr);
    ~EditMainView();

    bool init();

    /**
     * @brief ��ʼ������
     * @param [in] type : ��������(GLOBALDEF::NODETYPE)
     * @param [in] data : ����
     */
    void initData(int type, void* data);

    // ��ѯ��ǰ��ʾ��������Ϣ
    int queryTableInformation(const QString &type) const;

    // ��ѯ���ݳ���ƫ����
    int queryLengthOffset(const ICDMetaData::smtMeta &meta) const;

    // ��ѯ�Ѵ��ڱ��е�����
    QMap<QString, QString> queryExistedData(const QString &section) const;

    // ��ѯ֡����
    QMap<int, QString> queryFrameData(const QString &type) const;

    // ���bit�������ݵ���ʼ�ֽںź����
    void fillBitSerial(ICDMetaData::smtMeta &meta);

    // ��ѯ����ؼ�״̬
    bool queryWidgetState(const QString &name) const;

    // ������������ģ�������
    void dealCommand(int command, const QVariant &param);

signals:

public slots:

private:
    // ������ʾ�ܹ���Ϣ
    JLRESULT showInfrastructure(const Icd::JNEvent &event);
    // ��ѯ����ͼ��������Ϣ
    JLRESULT queryTableInformation(const Icd::JNEvent &event);
    // ��ѯ���ݳ���ƫ��
    JLRESULT queryLengthOffset(const Icd::JNEvent &event);
    // ��ѯ��ǰ�����������ݣ���������ǰ�����ݣ�
    JLRESULT queryExistedData(const Icd::JNEvent &event);
    // ��ѯ��ǰ���е�֡����
    JLRESULT queryFrameData(const Icd::JNEvent &event);
    // ��������ź��ֽ����
    JLRESULT fillBitSerial(const Icd::JNEvent &event);
    // ��ѯ����ͼ�ؼ�״̬
    JLRESULT queryMainViewState(const Icd::JNEvent &event);
    // ��������Navigationģ�������
    JLRESULT dealNavigationCommand(const Icd::JNEvent &event);

private:
    DataEditWidget *q_dataWidget;      // ��Ϣ�༭����
};

#endif // EDITMAINVIEW_H
