#ifndef AXISSEL_WIDGET_H
#define AXISSEL_WIDGET_H

#include <QLineEdit>
#include <QListWidget>
#include <QDialog>

// class AxisXDropLineEdit

class AxisXDropLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit AxisXDropLineEdit(QWidget *parent = 0);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
};

// class AxisYDropListWidget

class AxisYDropListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AxisYDropListWidget(QWidget *parent = 0);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
};

// class AxisSelWidget

class QLineEdit;
class QComboBox;
class QListWidget;

class AxisSelWidget : public QDialog
{
    Q_OBJECT
public:
    explicit AxisSelWidget(QWidget *parent = 0);
    ~AxisSelWidget();

    /**
     * @brief setHasTimeFormat
     * @param valid
     */
    void setHasTimeFormat(bool valid);

    /**
     * @brief axisXType
     * @return
     */
    QVariant::Type axisXType() const;

    /**
     * @brief axisXDomain
     * @return
     */
    QString axisXDomain() const;

    /**
     * @brief axisYDomain
     * @return
     */
    QStringList axisYDomain() const;

Q_SIGNALS:

public Q_SLOTS:

protected:
    void accept();

private:
    AxisXDropLineEdit *d_editAxisX;
    QComboBox *d_comboBoxAxisX;
    AxisYDropListWidget *d_listWidgetAxisY;
    bool d_hasTimeFormat;
};

#endif // AXISSEL_WIDGET_H
