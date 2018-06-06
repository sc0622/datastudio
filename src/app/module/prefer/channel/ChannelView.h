#ifndef CHANNELVIEW_H
#define CHANNELVIEW_H

#include <QMainWindow>

class ComMgrWidget;

namespace Prefer {

class ChannelView : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelView(QWidget *parent = 0);
    ~ChannelView();

    bool init();

signals:

public slots:

private:
    ComMgrWidget *d_comMgrWidget;
};

}

#endif // CHANNELVIEW_H
