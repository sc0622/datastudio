#ifndef ICDMAINVIEW_H
#define ICDMAINVIEW_H

#include <QFrame>
#include "../datamgr/datamanagement.h"

class ICDMainViewUi;

class ICDMainView : public QFrame
{
    Q_OBJECT
public:
    explicit ICDMainView(QWidget *parent = nullptr);
    virtual ~ICDMainView();

    bool init();

signals:

public slots:

protected:
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
    ICDMainViewUi *q_ui;
};

#endif // ICDMAINVIEW_H
