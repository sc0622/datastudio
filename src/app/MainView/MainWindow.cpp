#include "precomp.h"
#include "MainWindow.h"
#include "MainStatusBar.h"
#include "CentralWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName(metaObject()->className());
    setDockOptions(QMainWindow::AnimatedDocks
                   | QMainWindow::AllowNestedDocks
                   | QMainWindow::AllowTabbedDocks);

    d_statusBar = new Main::StatusBar(this);
    setStatusBar(d_statusBar);

    d_centralWidget = new CentralWidget(this);
    setCentralWidget(d_centralWidget);
}

MainWindow::~MainWindow()
{
    JMain::saveWidgetState(this);
}

bool MainWindow::init()
{
    bool result = true;

    JMain::restoreWidgetState(this);
    d_centralWidget->init();
    d_statusBar->hide();

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
        showMaximized();
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
