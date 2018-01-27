#include "precomp.h"
#include "SplashWidget.h"

SplashWidget::SplashWidget(QWidget *parent)
    : QSplashScreen(parent)
{
    setFont(QFont("Microsoft Yahei", 24));

    QSize screenSize = QApplication::desktop()->screenGeometry().size();
    setPixmap(QPixmap(":/datastudio/image/startup.png")
              .scaled(screenSize.width() / 2, screenSize.height() / 2,
                      Qt::KeepAspectRatio, Qt::SmoothTransformation));

    showMessage(tr("Initializing..."), Qt::AlignBottom | Qt::AlignHCenter, QColor("steelblue"));
}
