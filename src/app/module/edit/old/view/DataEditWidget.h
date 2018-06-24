#ifndef DATAEDITWIDGET_H
#define DATAEDITWIDGET_H

#include "KernelClass/planenode.h"
#include "main_global.h"

class LoggingWidget;
class JXmlTable;
struct _UIData;
class JSplitter;
class QLabel;
class QAction;

class DataEditWidget : public QWidget
{
    Q_OBJECT
public:
    enum optType{
        optNew  = 0,    // �༭
        optEdit,        // �޸�
        optDelete,      // ɾ��
        optCopy,        // ����
    };
    enum {
        ComplexTable = Qt::UserRole + 5,
    };

    explicit DataEditWidget(QWidget *parent = nullptr);
    ~DataEditWidget();

    bool init();

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
    void updateOne(int index, const TableNode::smtTable &data, optType type);
    // ����
    void showData(const ICDMetaData::ruleMap &rules);
    void updateOne(int index, const ICDMetaData::smtMeta &data, optType type);
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

protected slots:
    // ��ǰѡ������
    void slotCurrentItemChanged(QStandardItem *current, QStandardItem *previous);
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
    void setActionEnabled(const QString &name, bool enabled = true);

private:
    JSplitter* splitterMain_;
    LoggingWidget *loggingWidget_;  // ��Ϣ¼�����
    JXmlTable *tableView_;          // ������Ϣ��
    QLabel *editStatus_;            // ״̬��ʾ
    QAction *actionPast_;           // �Ҽ��˵�ճ��

    const QString vehicleName_;     // ���ͱ�
    const QString systemName_;      // ��ϵͳ��
    const QString tableName_;       // icd��
    const QString ruleName_;        // �����
    const QString frameName_;       // ֡ʶ����
    const QString objectName_;      // ������
    QString defaultPath_;           // �����ļ�Ĭ��·��

    int dataType_;                  // ��������
    int subType_;                   // ������
    int newIndex_;                  // ��¼���������к�
    int changedPos;                 // ���ݱ����ʼֵ����0��ʼ
    ICDElement::elementVector vData_;   // ��ǰ��ʾ��Ϣ
    ICDElement::smtElement data_;       // ��ǰ��ʾ������Ϣ
};

#endif // DATAEDITWIDGET_H
