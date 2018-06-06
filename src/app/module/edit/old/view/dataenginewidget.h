#ifndef BASICTABLEWIDGET_H
#define BASICTABLEWIDGET_H

#include "KernelClass/planenode.h"

#include <QtWidgets>

class LoggingWidget;
class JXmlTable;
struct _UIData;
/**
 * @brief ���ݱ༭�����ڣ����ݲ�ͬ���������ͣ����ء��༭��Ӧ��Ϣ
 */
class DataEngineWidget
        : public QWidget
{
    enum optType{
        optNew  = 0,    // �༭
        optEdit,        // �޸�
        optDelete,      // ɾ��
        optCopy,        // ����
    };
    enum {
        ComplexTable = Qt::UserRole + 5,
    };
    Q_OBJECT
public:
    DataEngineWidget(QWidget *parent = nullptr);
    ~DataEngineWidget();

    // ��ʼ������
    void initUI(int type, void *data);
    // ������ʼ��
    void reInit();
    // ��ѯ��ǰ��ʾ��������Ϣ
    int queryTableInformation(const QString &type) const;
    // ��ѯ�Ѵ��ڱ��е�����
    QMap<QString, QString> queryExistedData(const QString &section) const;
    // ��ѯ֡����
    QMap<int, QString> queryFrameData(const QString &type) const;
    // ���bit�������ݵ���ʼ�ֽں�
    void fillBitIndex(ICDMetaData::smtMeta &meta);
    // ��ѯ���ݳ���ƫ����
    int queryOffset(const ICDMetaData::smtMeta &meta) const;
    // ��ѯ����ؼ�״̬
    bool queryWidgetState(const QString &name) const;
    // ������������ģ�������
    void dealCommand(int command, const QVariant &param);
    //
    void setMenuPtr(const std::string &name, QAction *action);
    //
    bool dealMenuCmd(const std::string &name);

protected:
    /**������ʾ����**/
    // ����
    void showData(const PlaneNode::planeVector &planes);
    void updateOne(int index, const PlaneNode::smtPlane &data, optType type);
    // ϵͳ
    void showData(const SystemNode::systemVector &systems);
    void updateOne(int index, const SystemNode::smtSystem &data, optType type);
    // ICD��
    void showData(const TableNode::tableVector &tables);
    void updateOne(int index, const TableNode::smtTable &data,
                   optType type);
    // ����
    void showData(const ICDMetaData::ruleMap &rules);
    void updateOne(int index, const ICDMetaData::smtMeta &data,
                   optType type);
    // ����������
    void showData(const ICDMetaData::smtMeta &data);
    // �ӱ�
    void updateSubOne(int index, const stICDBase &data, optType type);
    // ��������
    void updateMetaOne(const ICDMetaData::smtMeta &data);
    /**������ʾ����**/

    // ���±༭���ݴ���
    void updateLoggingUI(GlobalDefine::OptionType option);

    // ��ȡ��ǰѡ��������
    ICDElement::smtElement currentData() const;
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // ��������
    void newPlane();
    // ���»��ͱ༭���ݽ���
    void updatePlaneUI(GlobalDefine::OptionType option);
    // ����ϵͳ
    void newSystem();
    // ����ϵͳ�༭���ݽ���
    void updateSystemUI(GlobalDefine::OptionType option);
    // ����ICD��
    void newICDTable();
    // ����ICD��༭���ݽ���
    void updateICDTableUI(GlobalDefine::OptionType option);
    // ��������
    void newICDRule(int type);
    // ���¹���༭���ݽ���
    void updateICDRuleUI(GlobalDefine::OptionType option);
    // ��������¼�����
    void updateDetailUI(const _UIData &data);
    // �����ӱ�
    void newSubTable();
    // �����ӱ�༭���ݽ���
    void updateSubTableUI(GlobalDefine::OptionType option);
    // ��/��״̬��ʾ
    void setTipsVisible(bool visible);
    // ��ѯ��ǰѡ��������bitλ�ĵ�һ���������
    int queryFirstBitSerial(int bitIndex);
    // ��ѯ�ܷ񽫵�ǰ���ݺϲ���ǰһ����ߺ�һ������
    bool canMerged(const ICDMetaData::smtMeta &meta) const;
    // ��ʾ�Ҽ��˵�
    void showMenu();
    // ��ǰ���Ƿ�����ճ�����Ƶ�����
    bool canPasted() const;
    // ɾ������
    void deleteRule(int pos);
    // ��������
    void insertRule(const ICDMetaData::smtMeta &meta, int pos);
    // �������λ��
    void movePlane(int from, int to);
    // ���ϵͳλ��
    void moveSystem(int from, int to);
    // �����λ��
    void moveTable(int from, int to);
    // �������λ��
    void moveRule(int from, int to);
    // ����ӱ�λ��
    void moveSubTable(int from, int to);
    // �������ݵ�����
    void saveEditData(void *data);
    // ���¼�����������ֽ����
    void reorderTable(TableNode::smtTable &table, int from);
    // ��ѯ��ǰ�������ݸ���
    TableNode::smtTable parentTable() const;
    //
    void setActionState(QAction *act, bool enabled);

protected slots:
    // ��ǰѡ������
    void slotCurrentItemChanged(QStandardItem *current,
                                QStandardItem *previous);
    // �����������
    void slotItemPressed(QStandardItem *item);
    // ���Զ���˵�
    void slotCustomContextMenu(const QPoint &pos);
    // �����ݱ��
    void slotContentChanged();
    // ���������λ��
    void slotDataPosChanged(int logical, int oldIndex, int newIndex);
    // ����������ݵ��ڴ�
    void slotSave2Memory(void *data, bool &result);
    // ȡ��
    void slotCanceled();
    // ����
    void slotNew(const QVariant &param);
    // ����
    void slotInsert();
    // ����
    void slotMoveUp();
    // ����
    void slotMoveDown();
    // ɾ��
    void slotDelete();
    // ���
    void slotClear();
    // �������ݵ�����Դ
    void slotSave2Source(GlobalDefine::DataSource type);
    // ����
    void slotCopy();
    // ճ��
    void slotPaste();

private:
    LoggingWidget   *q_loggingWidget;   // ��Ϣ¼�����

    JXmlTable       *q_table;       // ������Ϣ��
    QGroupBox       *q_tableBox;    // ������Ϣ���
    QWidget         *q_tipWidget;   // ��ʾ����
    QLabel          *q_edtStatus;   // ״̬��ʾ

    QAction         *q_actNew;      // ������ť
    QAction         *q_actInsert;   // ���밴ť
    QAction         *q_actUp;       // ���ư�ť
    QAction         *q_actDown;     // ���ư�ť
    QAction         *q_actDelete;   // ɾ����ť
    QAction         *q_actClear;    // ��հ�ť
    QAction         *q_actSaveDB;   // ���浽���ݿ�
    QAction         *q_actSaveFile; // ���浽�ļ�
    QAction         *q_paste;       // �Ҽ��˵�ճ��

    QAction         *q_genTableGuid;// ���ɱ�GUID

    const QString   q_planeName;    // ���ͱ�
    const QString   q_systemName;   // ��ϵͳ��
    const QString   q_tableName;    // icd��
    const QString   q_ruleName;     // �����
    const QString   q_discertName;  // ֡ʶ����
    const QString   q_metaName;     // ������
    QString         q_defaultPath;  // �����ļ�Ĭ��·��

    int             q_dataType;     // ��������
    int             q_subType;      // ������
    int             q_newIndex;     // ��¼���������к�
    int             q_changePos;    // ���ݱ����ʼֵ����0��ʼ
    ICDElement::elementVector   q_vData;    // ��ǰ��ʾ��Ϣ
    ICDElement::smtElement      q_data;     // ��ǰ��ʾ������Ϣ
};

#endif // BASICTABLEWIDGET_H
