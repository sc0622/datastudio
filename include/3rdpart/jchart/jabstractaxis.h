#ifndef JABSTRACTAXIS_H
#define JABSTRACTAXIS_H

#include "jchart_global.h"
#include <QWidget>

namespace JChart {

class AbstractAxisPrivate;

/**
 * @brief The JAbstractAxis class
 */
class JCHART_EXPORT AbstractAxis : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief JAbstractAxis
     * @param parent
     * @param f
     */
    explicit AbstractAxis(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~AbstractAxis();

signals:

public slots:

private:
    Q_DISABLE_COPY(AbstractAxis)
    J_DECLARE_PRIVATE(AbstractAxis)
};

} // end of namespace JChart

#endif // JABSTRACTAXIS_H
