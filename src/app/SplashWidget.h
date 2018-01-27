#ifndef SPLASHWIDGET_H
#define SPLASHWIDGET_H

#include <QSplashScreen>

class SplashWidget : public QSplashScreen
{
    Q_OBJECT
public:
    explicit SplashWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
};

#endif // SPLASHWIDGET_H
