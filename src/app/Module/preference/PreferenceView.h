#ifndef PREFERENCEVIEW_H
#define PREFERENCEVIEW_H

#include <QDockWidget>

namespace Preference {

class ChannelView;

class View : public QDockWidget
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);
    ~View();

    bool init();

signals:

public slots:

private:
    ChannelView *d_channelView;
};

}

#endif // PREFERENCEVIEW_H
