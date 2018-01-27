#ifndef ANALYSEVIEW_H
#define ANALYSEVIEW_H

#include <QWidget>

class QToolBar;
class JSplitter;

namespace Json { class Value; }

namespace Analyse {

class TreeView;
class ChartView;

class View : public QWidget
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = nullptr);
    ~View();

    bool init();

signals:

public slots:

private:
    void updateToolBar();
    void addViewAction(const Json::Value &option);
    void addSettingsAction();

    void convertRecordData();

private:
    QToolBar *d_toolBar;
    JSplitter *d_splitterMain;
    TreeView *d_treeView;
    ChartView *d_chartView;
};

}

#endif // ANALYSEVIEW_H
