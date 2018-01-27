#ifndef MAINNAVWIDGET_H
#define MAINNAVWIDGET_H

#include <QWidget>
#include <QTreeWidget>

namespace Icd {

class CoreTreeWidget;

} // end of namespace Icd

// class MainNavWidget

class QTreeWidgetItem;

class MainNavWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief MainNavWidget
     * @param parent
     */
    explicit MainNavWidget(QWidget *parent = 0);

    /**
     * @brief init
     * @return
     */
    bool init();

    /**
     * @brief releaseWork
     */
    void releaseWork();

    /**
     * @brief setLoadingDeep
     * @param deep
     */
    void setLoadingDeep(int deep);

    void showItemOffset(bool checked);
    void showItemType(bool checked);
    void showItemData(bool checked);
    void setDataFormat(int format);
    void showItemValue(bool checked);
    void showItemSpec(bool checked);
    void setPlay(bool running);
    int intervalUpdate() const;
    void setIntervalUpdate(int interval);

    // batch process
    void bindingChannels();
    void unbindingChannels();
    void exportBindingStatus();
    void runAllChannels();
    void stopAllChannels();

    /**
     * @brief updateUi
     */
    void updateUi();

signals:

public slots:

private:
    Icd::CoreTreeWidget *d_icdTree;
};

#endif // MAINNAVWIDGET_H
