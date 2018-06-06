#ifndef ICDNAVIGATION_H
#define ICDNAVIGATION_H

#include <QWidget>
#include "../datamgr/datamanagement.h"

class ICDNavigationUi;

class ICDNavigation : public QWidget
{
    Q_OBJECT
public:
    explicit ICDNavigation(QWidget *parent = nullptr);
    virtual ~ICDNavigation();

signals:

public slots:

protected:
    // ��ѯ�ڵ�����
    JLRESULT queryNodeData(const Icd::JNEvent &event);
    // ��ѯ��ǰ�ڵ㼰ֱ�����ڵ�Ĺؼ��뼯
    JLRESULT queryNodeKeys(const Icd::JNEvent &event);
    // ��ѯ�ڵ����״̬
    JLRESULT queryNodeFlag(const Icd::JNEvent &event);
    // ��ѯ�ڵ㶥���
    JLRESULT queryTopTable(const Icd::JNEvent &event);
    // ��ѯ����ID
    JLRESULT queryID(const Icd::JNEvent &event);
    // ���½ڵ�����
    JLRESULT updateNodeData(const Icd::JNEvent &event);
    // ����ڵ�����
    JLRESULT insertNodeData(const Icd::JNEvent &event);
    // ɾ���ڵ�
    JLRESULT deleteNode(const Icd::JNEvent &event);
    // ���µ����ڵ�˳��
    JLRESULT reorderNodeData(const Icd::JNEvent &event);
    // ����ճ������
    JLRESULT savePastedData(const Icd::JNEvent &event);
    // �������ݵ�����Դ
    JLRESULT saveMemoryData(const Icd::JNEvent &event);
    // ���¼��ؽ�������
    JLRESULT reloadUiData(const Icd::JNEvent &event);
    // ģʽ�л�
    JLRESULT onModuleChanged(const Icd::JNEvent &event);
    // �˳�ϵͳ
    JLRESULT onSystemQuit(const Icd::JNEvent &event);

private:
    ICDNavigationUi *q_ui;
};

#endif // ICDNAVIGATION_H
