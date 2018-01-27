#ifndef XMLSETTINGWIDGET_H
#define XMLSETTINGWIDGET_H

#include <QtWidgets>

class XmlSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit XmlSettingWidget(IJAttempter &attempter, QWidget *parent = 0);

    // ��ʼ����������
    bool initUi();
    //
    bool dataValid();
    //
    QString xmlFile() const;

Q_SIGNALS:
    void configChanged();

public slots:
    void slotApplyButtonClicked();
    void slotScanButtonClicked();

protected:
    // ����xml��Ϣ
    bool updateXml();
    //
    void enableButtons(bool enabled);

private:
    IJAttempter     &d_attempter;
    QLineEdit       *d_file;        // xml�ļ�
    QPushButton     *d_buttonApply; // Ӧ��
    QPushButton     *d_buttonCancel;// ȡ��
    bool            d_valid;    // ������Ч��־
};

#endif // XMLSETTINGWIDGET_H
