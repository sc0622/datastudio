#ifndef JCHART_VIEWMODEL_H
#define JCHART_VIEWMODEL_H

#include "jchart_global.h"

namespace JChart {

class ViewModelPrivate;
class Chart;

/**
 * @brief The ViewModel class
 */
class JCHART_EXPORT ViewModel
{
public:
    /**
     * @brief ViewModel
     */
    explicit ViewModel();

    ~ViewModel();

    /**
     * @brief chartAt
     * @param x
     * @param y
     * @return
     */
    Chart *chartAt(int x, int y) const;

    /**
     * @brief chartAt
     * @param pos
     * @return
     */
    inline Chart *chartAt(const QPoint &pos) const;

private:
    J_DECLARE_PRIVATE(ViewModel)
    friend class ViewPrivate;
};

Chart *ViewModel::chartAt(const QPoint &pos) const
{
    return chartAt(pos.x(), pos.y());
}

} // end of namespace JChart

#endif // JCHART_VIEW_MODEL_H
