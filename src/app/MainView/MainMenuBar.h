#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <QMenuBar>

namespace Main {

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit MenuBar(QWidget *parent = nullptr);

    bool init();

signals:

public slots:

private:
    void updateMenuBar();
    void addSettingsMenu();
    void addAnalyseToolAction();
    void addCalcToolAction();
    void addScreenshotAction();
    void addPaletteAction();
    void addShowToolbarAction();

private:
};

}

#endif // MAINMENUBAR_H
