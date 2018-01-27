#ifndef JCHART_VIEW_H
#define JCHART_VIEW_H

#include <QWidget>
#include "jchart_global.h"
#include "jchart_viewmodel.h"

class JTableView;

namespace JChart {

class ViewPrivate;
class Chart;

/**
 * @brief 图表视图类
 */
class JCHART_EXPORT View : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount NOTIFY columnCountChanged)
public:
    /**
     * @brief 构造函数
     * @param parent : 父窗口
     */
    explicit View(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    /**
     * @brief View
     * @param styled
     * @param parent
     * @param f
     */
    explicit View(bool styled, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~View();

    /**
     * @brief 追加一个图表
     * @param chart : 图表
     */
    void appendChart(Chart *chart);

    /**
     * @brief 插入一个图表
     * @param index : 插入位置索引
     * @param chart : 图表
     */
    void insertChart(int index, Chart *chart);

    /**
     * @brief 删除一个图表，并自动销毁
     * @param chart : 图表
     * @return remove row flag
     */
    bool removeChart(Chart *chart);

    /**
     * @brief removeChart
     * @param row
     * @param column
     * @return remove row flag
     */
    bool removeChart(int row, int column);

    /**
     * @brief 清空图表容器
     */
    void clearChart();

    /**
     * @brief chartCount
     * @return
     */
    int chartCount() const;

    /**
     * @brief 获取指定位置的图表
     * @param index : 索引
     * @return 图表
     */
    Chart *chartAt(int index) const;

    /**
     * @brief chartAt
     * @param row
     * @param column
     * @return
     */
    Chart *chartAt(int row, int column) const;

    /**
     * @brief focusChart
     * @param index
     */
    void focusChart(int index);

    /**
     * @brief focusChart
     * @param row
     * @param column
     */
    void focusChart(int row, int column);

    /**
     * @brief focusChart
     * @param identity
     */
    void focusChart(const QString &identity);

    /**
     * @brief focusChart
     * @param chart
     */
    void focusChart(Chart *chart);

    /**
     * @brief 获取指定图表的位置索引
     * @param chart : 图表
     * @return 指定图表的位置索引
     */
    int indexOfChart(Chart *chart);

    /**
     * @brief 获取图表容器
     * @return 图表容器
     */
    QList<Chart *> charts() const;

    /**
     * @brief model
     * @return
     */
    ViewModel *model() const;

    /**
     * @brief isStyled
     * @return
     */
    bool isStyled() const;

    /**
     * @brief rowCount
     * @return
     */
    int rowCount() const;

    /**
     * @brief columnCount
     * @return
     */
    int columnCount() const;

    /**
     * @brief setColumnWidthSame
     */
    void setColumnWidthSame();

signals:
    void triggleClose(JChart::Chart *chart);
    void columnCountChanged(int);
    void seriesRemoved(int row, int column);
    void trackerChanged(JChart::Chart *chart, const QPointF &pos, bool visible);
    void trackerMarked(JChart::Chart *chart, const QPointF &pos);

public slots:
    void setColumnCount(int count);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(View)
    J_DECLARE_PRIVATE(View)
};

} // end of namesoace JChart

#endif // JCHART_VIEW_H
