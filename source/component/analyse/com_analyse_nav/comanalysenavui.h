#ifndef COMANALYSENAVUI_H
#define COMANALYSENAVUI_H

#include <QWidget>

class IJAttempter;
class MainNavWidget;

class ComAnalyseNavUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComAnalyseNavUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComAnalyseNavUi();

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
     * @brief updateUi
     */
    void updateUi();

    void showItemOffset(bool checked);
    void showItemType(bool checked);
    void loadIcdFile();
    void unloadDataFile();

Q_SIGNALS:

public Q_SLOTS:

private:
    IJAttempter &d_attempter;
    MainNavWidget *d_navWidget;
};

#endif // COMANALYSENAVUI_H
