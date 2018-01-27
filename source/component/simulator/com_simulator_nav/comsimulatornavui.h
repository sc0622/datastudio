#ifndef COMSIMULATORNAVUI_H
#define COMSIMULATORNAVUI_H

#include <QWidget>

class IJAttempter;
class MainNavWidget;

class ComSimulatorNavUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComSimulatorNavUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComSimulatorNavUi();

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

Q_SIGNALS:

public Q_SLOTS:

private:
    IJAttempter &d_attempter;
    MainNavWidget *d_navWidget;
};

#endif // COMSIMULATORNAVUI_H
