#ifndef XMLSETTINGWIDGET_H
#define XMLSETTINGWIDGET_H

#include <QtWidgets>

class XmlSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit XmlSettingWidget(IJAttempter &attempter, QWidget *parent = 0);

    // 初始化界面数据
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
    // 更新xml信息
    bool updateXml();
    //
    void enableButtons(bool enabled);

private:
    IJAttempter     &d_attempter;
    QLineEdit       *d_file;        // xml文件
    QPushButton     *d_buttonApply; // 应用
    QPushButton     *d_buttonCancel;// 取消
    bool            d_valid;    // 数据有效标志
};

#endif // XMLSETTINGWIDGET_H
