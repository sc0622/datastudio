#ifndef ANALYSEWINDOW_H
#define ANALYSEWINDOW_H

#include <QWidget>

class JSplitter;

namespace Json { class Value; }

namespace Analyse {

class TreeView;
class ChartView;

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

    bool init();

signals:

public slots:

private:

    void convertRecordData();

private:
    JSplitter *splitterMain_;
    TreeView *treeView_;
    ChartView *chartView_;
};

}

#endif // ANALYSEWINDOW_H
