#ifndef LOGGINGWIDGET_H
#define LOGGINGWIDGET_H

#include "ObjectEdit.h"

class QGroupBox;
class QComboBox;
class QStackedWidget;

class LoggingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoggingWidget(QWidget *parent = nullptr);

    // ��ʼ����������
    void initUIData(int windowType, const _UIData &data);
    // ��ȡ��������
    void *uiData();
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

signals:
    void dataSaved(void *data, bool &result);
    void canceled();

public slots:
    // ��������
    void slotConfirm(bool &result);
    // ȡ��
    void slotCancel();
    // �������ͱ��
    void slotTypeChanged(int index);

private:
    // ��ʼ������������Ϣ
    void initTypeInfo(const _UIData &data);
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);

private:
    QGroupBox *groupType_;
    QComboBox *boxType_;
    QStackedWidget *stackedWidget_;
    ObjectEdit *currentEdit_;
};

#endif // LOGGINGWIDGET_H
