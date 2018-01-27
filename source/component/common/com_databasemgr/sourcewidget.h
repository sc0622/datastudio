#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <QtWidgets>

class SourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SourceWidget(QWidget *parent = 0);

    // ��ʼ����������
    void initUi();

    /**
     * @brief sourceType
     * @return
     */
    QString sourceType() const;

    // ��ԭ����Դ
    void restore();

    // ���ý��水ť״̬
    void setApplyState(bool enabled);

signals:
    void sourceChanged(bool &);
    void typeChanged(const QString &text);

public slots:
    void slotApplyButtonClicked();
    void slotTypeChanged(const QString &text);

private:
    // ��ȡ����Դ��������
    bool readSourceType(QHash<QString, QString> &type, QString &currentType);
    // ��������Դ
    bool updateSoure();

private:
    QComboBox       *d_box;       // ����Դ������
    QPushButton     *d_buttonApply; // Ӧ��
    int             d_lastIndex;    // ��¼�ϴ���Ч������Դ
};

#endif // SOURCEWIDGET_H
