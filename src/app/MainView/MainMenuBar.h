#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <QMenuBar>

namespace Json {class Value; }

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
    void updateMenuBar(const Json::Value &config);
    void addSettingsMenu(const Json::Value &config);
    void addAnalyseToolAction(const Json::Value &config);
    void addCalcToolAction(const Json::Value &config);
    void addScreenshotAction(const Json::Value &config);
    void addPaletteAction(const Json::Value &config);
    void addShowToolbarAction(const Json::Value &config);

private:
};

}

#endif // MAINMENUBAR_H
