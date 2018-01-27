#ifndef JDIALOG_H
#define JDIALOG_H

#include "jwt_global.h"
#include <QDialog>
#include <QMenu>

// - class JDialog -

class JDialogPrivate;

class JWT_EXPORT JDialog : public QDialog
{
    Q_OBJECT
public:
    explicit JDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~JDialog();

    QString windowTitle() const;

    QIcon windowIcon() const;
    void setWindowIcon(const QIcon &icon);

    QLayout *layout() const;
    void setLayout(QLayout *layout);
    void setWindowTitleFont(const QFont &font);
    void setTitleAlignment(Qt::Alignment alignment);
    QWidget *clientWidget() const;
    void setClientWidget(QWidget *widget);
    QRect clientRect() const;

    //void setMainMenu(QMenu *menu);
    //void addTitleWidget(QWidget *widget, const QString &objectName = QString());
    //void addTitleSpacing(int size);
    //void removeTitleWidget(QWidget *widget);

    //void transparentClientWidget(bool transparent);
    //void setStatusWidget(bool enable = true);

    void setCenter(QWidget *parent = 0);
    void enableShadow(bool enable = true);

    //
    QPair<QColor/*top*/, QColor/*bottom*/> titleColor() const;
    void setTitleColor(const QColor &top, const QColor &bottom);

    QColor borderColor() const;
    void setBorderColor(const QColor &color);

    QColor clientColor() const;
    void setClientColor(const QColor &color);

public Q_SLOTS:
    void show();
    void showMaximized();
    void showFullScreen();
    void showNormal();
    void setWindowTitle(const QString &text);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    JDialogPrivate *d;
};

#endif // JDIALOG_H
