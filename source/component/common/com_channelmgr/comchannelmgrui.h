#ifndef COMCHANNELMGRUI_H
#define COMCHANNELMGRUI_H

#include <QWidget>

class IJAttempter;
class ComMgrWidget;

class ComChannelMgrUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComChannelMgrUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComChannelMgrUi();

    void releaseWork();

Q_SIGNALS:

public Q_SLOTS:

private:
    IJAttempter &d_attempter;
    ComMgrWidget *d_comgrWidget;
};

#endif // COMCHANNELMGRUI_H
