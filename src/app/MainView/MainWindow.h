#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "main_global.h"

namespace Main {
class ToolBar;
class StatusBar;
}

class CentralWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool init();
    void dispose();

signals:

public slots:

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Main::StatusBar *d_statusBar;
    CentralWidget *d_centralWidget;
};

#endif // MAINWINDOW_H
