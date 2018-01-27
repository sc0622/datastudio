#ifndef COMDATABASEMGRUI_H
#define COMDATABASEMGRUI_H

#include <QtWidgets>

class IJAttempter;
class SourceWidget;
class DatabaseSettingWidget;
class XmlSettingWidget;

class ComDatabaseMgrUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComDatabaseMgrUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComDatabaseMgrUi();

    /**
     * @brief sourceType
     * @return
     */
    QString sourceType() const;

    /**
     * @brief filePath
     * @return
     */
    QString filePath() const;

Q_SIGNALS:

public Q_SLOTS:
    // �л�����Դ����
    void slotSourceTypeChanged(const QString &text);
    // ��������Դ�����¼�������
    void slotSourceChanged(bool &result);
    // �������Դ������Ϣ
    void slotConfigChanged();

private:
    IJAttempter &d_attempter;
    SourceWidget            *d_sourceWidget;
    QStackedWidget          *d_stackedWidget;
    DatabaseSettingWidget   *d_databaseWidget;
    XmlSettingWidget        *d_xmlWidget;
};

#endif // COMDATABASEMGRUI_H
