#ifndef JSKINBUTTON_H
#define JSKINBUTTON_H

#include "jwt_global.h"
#include <QWidget>

class JWT_EXPORT JSkinButton : public QWidget
{
    Q_OBJECT
public:
    explicit JSkinButton(const QString &name, QPixmap &pixmap, QWidget *parent = 0);

Q_SIGNALS:
    void clicked(bool checked = false);

private Q_SLOTS:
    void slotThisClicked(bool checked);

protected:
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    
private:
    bool            q_mouseEnter;
    int             q_downloadCount;    //
};

#endif // JSKINBUTTON_H
