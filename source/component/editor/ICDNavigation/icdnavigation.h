#ifndef ICDNAVIGATION_H
#define ICDNAVIGATION_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ICDNavigationUi;

class ICDNavigation
        : public IJComponent
        , public IJComponentUi
        , public JObserver
{
public:
    explicit ICDNavigation(IJAttempter* attemper);
    virtual ~ICDNavigation();

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

    // JObserver interface
public:
    std::string observerId() const;

protected:
    // ��ѯ�ڵ�����
    JLRESULT queryNodeData(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // ��ѯ��ǰ�ڵ㼰ֱ�����ڵ�Ĺؼ��뼯
    JLRESULT queryNodeKeys(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // ��ѯ�ڵ����״̬
    JLRESULT queryNodeFlag(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // ��ѯ�ڵ㶥���
    JLRESULT queryTopTable(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // ��ѯ����ID
    JLRESULT queryID(const std::string &id,
                     JWPARAM wParam,
                     JLPARAM lParam);
    // ���½ڵ�����
    JLRESULT updateNodeData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // ����ڵ�����
    JLRESULT insertNodeData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // ɾ���ڵ�
    JLRESULT deleteNode(const std::string &id,
                        JWPARAM wParam,
                        JLPARAM lParam);
    // ���µ����ڵ�˳��
    JLRESULT reorderNodeData(const std::string &id,
                             JWPARAM wParam,
                             JLPARAM lParam);
    // ����ճ������
    JLRESULT savePastedData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // �������ݵ�����Դ
    JLRESULT saveMemoryData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // ���¼��ؽ�������
    JLRESULT reloadUiData(const std::string &id,
                          JWPARAM wParam,
                          JLPARAM lParam);
    // ģʽ�л�
    JLRESULT onModuleChanged(const std::string &id,
                             JWPARAM wParam,
                             JLPARAM lParam);
    // �˳�ϵͳ
    JLRESULT onSystemQuit(const std::string &id,
                          JWPARAM wParam,
                          JLPARAM lParam);

private:
    IJAttempter *q_attempter;
    ICDNavigationUi *q_ui;
};

#endif // ICDNAVIGATION_H
