#ifndef JSPLITTER_H
#define JSPLITTER_H

#include "jwt_global.h"
#include <QSplitter>

// - class JSplitterHandle -

class JSplitterHandleData;
class JSplitter;

class JWT_EXPORT JSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
public:
    JSplitterHandle(Qt::Orientation o, QSplitter *parent = 0);
    virtual ~JSplitterHandle();

    QColor color() const;
    void setColor(const QColor &color);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);

    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isAlwaysVisible() const;
    void setAlwaysVisible(bool visible);

Q_SIGNALS:
    void handleClicked(bool checked);

public Q_SLOTS:
    virtual void setVisible(bool visible);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    JSplitterHandleData *d;
};

// - class JSplitter -

class JSplitterPrivate;

class JWT_EXPORT JSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit JSplitter(QWidget *parent = 0);
    explicit JSplitter(Qt::Orientation orientation, QWidget *parent = 0);
    JSplitter(const QList<double> &scales, QWidget *parent = 0);
    JSplitter(const QList<double> &scales, Qt::Orientation orientation, QWidget *parent = 0);
    virtual ~JSplitter();

    QList<double> scales() const;
    void setScales(const QList<double> &scales);

    QColor handleColor(int index = -1) const;
    void setHandleColor(const QColor &color, int index = -1);

    QPixmap handlePixmap(int index = -1) const;
    void setHandlePixmap(const QPixmap &pixmap, int index = -1);

    bool isCheckable(int index = -1) const;
    void setCheckable(bool enable, int index = -1);

    bool isAlwaysVisible(int index = -1) const;
    void setAlwaysVisible(bool visible, int index = -1);

    JSplitterHandle *handle(int index = -1) const;

Q_SIGNALS:
    void handleChanged(int index, bool checked);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    QSplitterHandle *createHandle();
    void updatesSizes();

private Q_SLOTS:
    void _emit_splitterMoved(int pos, int index);

private:
    Q_DISABLE_COPY(JSplitter)
    J_DECLARE_PRIVATE(JSplitter)
    friend class JSplitterHandle;
};

#endif // JSPLITTER_H
