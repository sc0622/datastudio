#ifndef MONITORCHARTVIEW_H
#define MONITORCHARTVIEW_H

#include <QWidget>

class QStandardItem;

namespace Icd { class ChartView; }

namespace Monitor {

class ChartView : public QWidget
{
    Q_OBJECT
public:
    explicit ChartView(QWidget *parent = nullptr);
    ~ChartView();

    bool init();

    void setXAxisSync(bool checked);
    void showYLabel(bool checked);
    void showYAlign(bool checked);
    void setSyncTrack(bool checked);
    void setFlushEnabled(bool checked);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    Icd::ChartView *d_chartView;
};

}

#endif // MONITORCHARTVIEW_H
