#ifndef ICDMAINVIEWUI_H
#define ICDMAINVIEWUI_H

#include <QWidget>
#include "KernelClass/planenode.h"

class IJAttempter;
class DataEngineWidget;

class ICDMainViewUi : public QWidget
{
    Q_OBJECT
public:
    explicit ICDMainViewUi(QWidget *parent = nullptr);
    ~ICDMainViewUi();

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

    //
    void setMenuPtr(const std::string &name, QAction *acttion);

    //
    bool dealMenuCmd(const std::string &name);

Q_SIGNALS:

public Q_SLOTS:

private:
    DataEngineWidget    *q_dataWidget;      // ��Ϣ�༭����
};

#endif // ICDMAINVIEWUI_H
