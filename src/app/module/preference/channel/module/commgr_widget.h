#ifndef COMMGR_WIDGET_H
#define COMMGR_WIDGET_H

#include <QWidget>
#include "icdworker/icdworker_pool.h"

// class ComMgrWidget;

class ComMgrWidgetPrivate;
class QListWidgetItem;

/**
 * @brief The ComMgrWidget class
 */
class ComMgrWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief ComMgrWidget
     * @param parent
     */
    explicit ComMgrWidget(QWidget *parent = 0);

    ~ComMgrWidget();

    /**
     * @brief updateUi
     */
    bool init();

signals:

public slots:

private:
    bool loadChannelConfig();

private:
    Q_DISABLE_COPY(ComMgrWidget)
    J_DECLARE_PRIVATE(ComMgrWidget)
};

#endif // COMMGR_WIDGET_H
