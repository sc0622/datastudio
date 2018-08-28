#ifndef ICDNAVIGATIONUI_H
#define ICDNAVIGATIONUI_H

#include <QWidget>
#include "KernelClass/planenode.h"
#include "KernelClass/icdcomplexdata.h"

namespace Icd { class JSearchEdit; }
class JTreeView;
class QMenu;
class QStandardItem;

class ICDNavigationUi : public QWidget
{
    // �洢�������Ͷ���
    enum {
        UserKey     = Qt::UserRole + 50,// �������
        ComplexFlag,                    // ���Ϲ����ʶ
        LevelIndex,                     // �ڵ���
        ItemLoaded,                     // ��������״̬
        SubTable,                       // �ӱ������������ݣ�
        RuleDefine,                     // �������Ͷ���
        DataChanged,                    // ���ݱ����ʶ��δ���浽����Դ��
        DataSource,                     // ����Դ
    };
    enum {
        optUnload = 0,  // ж��
        optLoad         // ����
    };
    enum
    {
        StateReset,     // ����
        StateChanged,   // ���
    };
    Q_OBJECT
public:
    explicit ICDNavigationUi(QWidget *parent = nullptr);
    ~ICDNavigationUi();

    bool init();

    // ������ʾ����ϵͳ�ṹ����
    void loadData(const PlaneNode::planeVector &data, int source);

    // ������
    void updateRuleItems(const PlaneNode::smtPlane &plane);

    // ��ѯ�ڵ�����
    void queryNodeData(const QString &condition,
                       ICDElement::smtElement &element);

    // ��ѯ�ڵ�״̬
    int queryNodeFlag(const QString &type) const;

    // ��ѯ��ǰ�ڵ㶥���
    void queryTopTable(ICDElement::smtElement &element);

    // ��ѯ��ǰ�ڵ㼰ֱ�����ڵ�Ĺؼ��뼯
    QString queryCurrentKeys() const;

    // ��ѯ����Դ
    int queryDataSource() const;

    // ���½ڵ�����
    bool updateNodeData(int dataAddr);

    // ��ȡ���ظ���ID
    QString nextID(int type);

    // ���µ����ڵ�˳��
    bool insertNodeData(const std::vector<int> &params);

    // ɾ���ڵ�
    bool deleteNode(const std::vector<QString> &nodeID);

    // ����ڵ�����
    bool reorderNodeData(const std::vector<int> &params);

    // ����ճ������
    ICDElement::smtElement savePastedData(const QVector<int> &params);

    // �����ڴ����ݵ�����Դ
    bool saveMemoryData(int type, const QString &file, QString &err, bool saveAll = false);

Q_SIGNALS:

protected Q_SLOTS:
    // ������ڵ�
    void slotItemPressed(QStandardItem *item);

    // �л�ѡ�нڵ�
    void slotCurrentChanged(QStandardItem *current, QStandardItem *previous);

    // ���ع��������
    void slotLoadRule();

    // ж�ع��������
    void slotUnloadRule();

    // ���¹��������
    void slotUpdate();

    // ��������
    void slotNew();

    // ���������
    void slotClear();

    // ɾ������
    void slotDelete();

    // ��������
    void slotLoadData();
    void loadData(const QString &name);

    //
    bool loadVehicle();

private:
    // ���쵥�����ͽڵ㣨�����¹ҷ�ϵͳ��
    QStandardItem* formPlaneNode(QStandardItem *parent,
                                 const PlaneNode::smtPlane &plane);
    // ���µ������ͽڵ�˳��
    void reorderPlaneNode(const std::vector<ICDElement::smtElement> &planes, int pos);

    // ���쵥����ϵͳ�ڵ�
    QStandardItem* formSystemNode(QStandardItem *parent,
                                  const SystemNode::smtSystem& system);
    // ���µ���ϵͳ�ڵ�˳��
    void reorderSystemNode(const PlaneNode::smtPlane &plane, int pos);

    // ���쵥�������ڵ�
    QStandardItem* formTableNode(QStandardItem *parent,
                                 const TableNode::smtTable& table);
    // ���µ��������ڵ�˳��
    void reorderTableNode(const SystemNode::smtSystem &system, int pos);

    // ���쵥�������ӱ�ڵ�
    QStandardItem* formSubTableNode(QStandardItem *parent,
                                    const TableNode::smtTable& table, int index,
                                    int offset);
    // ���µ��������ӱ�ڵ�˳��
    void reorderSubTableNode(const ICDMetaData::smtMeta &meta, int pos);

    // �������ڵ�
    QStandardItem* formRuleNode(QStandardItem *parent,
                                const ICDMetaData::smtMeta& meta,
                                int offset = -1);
    // ���µ�������ڵ�˳��
    void reorderRuleNode(const TableNode::smtTable& table, int pos);

    // ���ع���ڵ�
    void loadRuleNode(QStandardItem *parent,
                      const TableNode::smtTable &table,
                      int offset = -1);

    // ж�ع�������
    void unLoadRuleNode(QStandardItem *item);

    // ���¼��ع���ڵ�
    void reLoadRuleNode(QStandardItem *item);

    // ��ʾ�Ҽ��˵�
    void showMenu(QStandardItem* item);

    // ��ѯָ���ڵ����ݼ���״̬
    int dataLoaded(QStandardItem* item) const;

    // ��ȡ��ǰ�ڵ��ֱ���ӽڵ�����
    QVector<QString> directChidData(QStandardItem* item) const;

    // ��ȡָ���ڵ���ӱ�
    std::vector<std::string> subTables(QStandardItem *item) const;

    // ��ѯ�ڵ��Ƿ����¹ҹ���ڵ�
    bool hasRuleTable(QStandardItem *item) const;

    // ��ѯ�ڵ��¹Ҷ�����������
    int topRuleTable(QStandardItem *item) const;

    // ��ȡ��ǰѡ�нڵ��ֱ�����ڵ�ؼ��루�����룬ϵͳ�룬������
    // level : Ŀ��ڵ�ȼ�
    QString parentKey(int level);
    /**��ȡ�ڵ�����**/
    PlaneNode::planeVector planeNodeMap();
    SystemNode::systemMap systemNodeMap(QStandardItem *item,
                                        PlaneNode::smtPlane plane);
    TableNode::tableMap tableNodeMap(QStandardItem *item,
                                     PlaneNode::smtPlane plane);
    TableNode::smtTable ruleNodeMap(QStandardItem *item,
                                    PlaneNode::smtPlane plane);
    TableNode::smtTable subRuleNodeMap(QStandardItem *item,
                                       PlaneNode::smtPlane plane);
    ICDMetaData::smtMeta ruleNodeData(QStandardItem *item,
                                      PlaneNode::smtPlane plane);

    // ����ָ�����ݲ�ѯ�ڵ�
    QStandardItem *findItem(int role, const QVariant &value,
                            QStandardItem *parent, bool direct = true) const;

    // �����Ѽ��ع����״̬
    void updateLoadedFlag(QStandardItem *item, int state);

    // ���¸��ڵ��Ѽ��ع���״̬
    void updateParentFlag(QStandardItem *item);

    // �����ӽڵ��Ѽ��ع���״̬
    void updateChildFlag(QStandardItem *item, int state);

    // ���½ڵ����ݸ���״̬
    void updateDataState(QStandardItem *item, int state);
    // ���¸��ڵ����ݸ���״̬
    void updateParentState(QStandardItem *item, int state);
    // �����ӽڵ����ݸ���״̬
    void updateChildState(QStandardItem *item, int state);

    // ��������ͼ����
    void updateMainView(QStandardItem *current);

    // �����������
    bool updatePalneData(stPlane &data);

    // ����ϵͳ����
    bool updateSystemData(stSystem &data);

    // ����ICD������
    bool updateICDData(stICDBase &data);

    // ����ICD��������
    bool updateRuleData(ICDMetaData::smtMeta &data, bool top = true);

    // ����ICD��������
    bool updateDetailRuleData(ICDMetaData::smtMeta &data);

    // �����ӱ�����
    bool updateSubTableData(stICDBase &data);

    // ��������ƫ�����ַ���
    QString offsetString(const ICDElement::smtElement &element, int offset = -1) const;

    // ��ѯ����ָ���ڵ����͵����ֵ
    void maxID(QStandardItem *parent, int level, QString &id);

    // ��ѯ�ڵ㼰ֱ�����ڵ�Ĺؼ��뼯
    QString queryNodeKeys(QStandardItem *item) const;

    // ����ڵ㼰���ӽڵ�
    void collapseItem(QStandardItem *item);

    // ������������˵�
    void createRuleMenu();

    // ���µ�ǰ�ڵ㶥����µ����й���ڵ���ʾ��Ϣ
    void updateTopTableTip();

    // ���¹���ڵ���ʾ��Ϣ
    void updateRuleTip(const TableNode::smtTable& table, QStandardItem *parent, int offset);

private:
    Icd::JSearchEdit *searchEdit_;
    JTreeView   *q_treeView;
    QMenu       *q_ruleMenu;
};

#endif // ICDNAVIGATIONUI_H
