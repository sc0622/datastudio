#include "precomp.h"
#include "MainWindow.h"
#include "MainTabWidget.h"
#include "MainToolBar.h"
#include "CentralWidget.h"
#include "MainStatusBar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName(metaObject()->className());
    setDockOptions(QMainWindow::AnimatedDocks
                   | QMainWindow::AllowNestedDocks
                   | QMainWindow::AllowTabbedDocks);

    tabWidget_ = new Main::TabWidget(this);
    setMenuWidget(tabWidget_);

    toolBar_ = new Main::ToolBar(this);
    toolBar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(toolBar_);

    centralWidget_ = new CentralWidget(this);
    setCentralWidget(centralWidget_);

    statusBar_ = new Main::StatusBar(this);
    setStatusBar(statusBar_);

    jnotify->on("menubar.fullscreen.toggled", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        static Qt::WindowStates oldStates = windowState();
        if (checked) {
            oldStates = windowState();
            setWindowState(Qt::WindowFullScreen);
        } else {
            setWindowState(oldStates);
        }
    });
}

MainWindow::~MainWindow()
{
    JMain::saveWidgetState(this);
}

bool MainWindow::init()
{
    bool result = true;

    JMain::restoreWidgetState(this);
    tabWidget_->init();
    centralWidget_->init();
    statusBar_->hide();

    return result;
}

void MainWindow::dispose()
{
    Icd::WorkerPool::getInstance()->stop();
    Icd::WorkerPool::getInstance()->clearWorker();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const int result = QMessageBox::warning(this,
                                            tr("Quit"),
                                            tr("Select buttons to continue..."),
                                            tr("Restart"),
                                            tr("Quit"),
                                            tr("Cancel"), 1, 2);
    switch (result) {
    case 0:     // restart
        JMain::instance()->setRestartFlag(true);
        event->accept();
        dispose();
        break;
    case 1:     // quit
        event->accept();
        dispose();
        break;
    case 2:     // cancel
        event->ignore();
        return;
    default:    // ignore
        break;
    }

    QMainWindow::closeEvent(event);
}
