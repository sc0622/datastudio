#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "main_global.h"

namespace Main {
class TabWidget;
class ToolBar;
class StatusBar;
}

class CentralWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
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
    Main::TabWidget *tabWidget_;
    Main::ToolBar *toolBar_;
    Main::StatusBar *statusBar_;
    CentralWidget *centralWidget_;
};

#endif // MAINWINDOW_H
