#ifndef LOGGINGWIDGET_H
#define LOGGINGWIDGET_H

#include <QtWidgets>

struct _UIData;
class MetaUI;

class LoggingWidget : public QWidget
{
    Q_OBJECT
public:
    enum WIDGEGTYPE {
        wdUnknown = -1,
        wdHeader,   // ��ͷ����
        wdCounter,  // ֡��������
        wdCheck,    // У�����
        wdFrameCode,// ֡ʶ����
        wdCommon,   // ͨ�ý���
        wdBitMap,   // bitMap����
        wdBitValue, // bitValue����
        wdDiscern,  // ֡���ݽ���
        wdComplex,  // �������ݽ���
        wdBuffer,   // ����Ԥ��������
        wdSubTable, // �ӱ����
        wdTable,    // �����
        wdSystem,   // ϵͳ����
        wdPlane,    // ���ͽ���
    };

public:
    explicit LoggingWidget(QWidget *parent = nullptr);
    // ���õ�ǰ��ʾ����
    void setCurrentWidget(WIDGEGTYPE type);
    // ��ʼ����������
    void initUIData(WIDGEGTYPE type, const _UIData &data);
    // ��ȡ��������
    void *uiData();
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

signals:
    void dataSaved(void *data, bool &result);
    void canceled();

protected slots:
    // ��������
    void slotConfirm(bool &result);
    // ȡ��
    void slotCancel();
    // �������ͱ��
    void slotTypeChanged(int index);

protected:
    // ��ʼ������������Ϣ
    void initTypeInfo(const _UIData &data);
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);

private:
    QGroupBox                   *q_groupType;
    QComboBox                   *q_boxType;
    QStackedWidget              *q_stack;
    QHash<WIDGEGTYPE, MetaUI*>  q_widget;
    QHash<int, WIDGEGTYPE>      q_relation;
};

#endif // LOGGINGWIDGET_H
