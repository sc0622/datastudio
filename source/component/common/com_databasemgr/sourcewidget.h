#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <QtWidgets>

class SourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SourceWidget(QWidget *parent = 0);

    // 初始化界面数据
    void initUi();

    /**
     * @brief sourceType
     * @return
     */
    QString sourceType() const;

    // 还原数据源
    void restore();

    // 设置界面按钮状态
    void setApplyState(bool enabled);

signals:
    void sourceChanged(bool &);
    void typeChanged(const QString &text);

public slots:
    void slotApplyButtonClicked();
    void slotTypeChanged(const QString &text);

private:
    // 读取数据源类型数据
    bool readSourceType(QHash<QString, QString> &type, QString &currentType);
    // 更新数据源
    bool updateSoure();

private:
    QComboBox       *d_box;       // 数据源下拉框
    QPushButton     *d_buttonApply; // 应用
    int             d_lastIndex;    // 记录上次有效的数据源
};

#endif // SOURCEWIDGET_H
