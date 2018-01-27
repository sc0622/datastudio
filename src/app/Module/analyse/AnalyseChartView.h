#ifndef ANALYSECHARTVIEW_H
#define ANALYSECHARTVIEW_H

#include <QWidget>

class QStandardItem;

namespace Icd { class ChartFileView; }

namespace Analyse {

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
    void setrReplayEnabled(bool checked);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    Icd::ChartFileView *d_chartView;
};

}

#endif // ANALYSECHARTVIEW_H
