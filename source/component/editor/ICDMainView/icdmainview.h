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
    // ������ʾ�ܹ���Ϣ
    JLRESULT showInfrastructure(const std::string &id,
                                JWPARAM wParam,
                                JLPARAM lParam);
    // ��ѯ����ͼ��������Ϣ
    JLRESULT queryTableInformation(const std::string &id,
                                   JWPARAM wParam,
                                   JLPARAM lParam);
    // ��ѯ���ݳ���ƫ��
    JLRESULT queryLengthOffset(const std::string &id,
                               JWPARAM wParam,
                               JLPARAM lParam);
    // ��ѯ��ǰ�����������ݣ���������ǰ�����ݣ�
    JLRESULT queryExistedData(const std::string &id,
                              JWPARAM wParam,
                              JLPARAM lParam);
    // ��ѯ��ǰ���е�֡����
    JLRESULT queryFrameData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // ��������ź��ֽ����
    JLRESULT fillBitSerial(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // ��ѯ����ͼ�ؼ�״̬
    JLRESULT queryMainViewState(const std::string &id,
                                JWPARAM wParam,
                                JLPARAM lParam);
    // ��������Navigationģ�������
    JLRESULT dealNavigationCommand(const std::string &id,
                                   JWPARAM wParam,
                                   JLPARAM lParam);

private:
    IJAttempter *q_attempter;
    ICDMainViewUi *q_ui;
};

#endif // ICDMAINVIEW_H
