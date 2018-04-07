#ifndef CONVERTTOICDWIDGET_H
#define CONVERTTOICDWIDGET_H

#include <QWidget>

class ConvertToIcdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConvertToIcdWidget(QWidget *parent = 0);

signals:
    void rejected();

public slots:
};

#endif // CONVERTTOICDWIDGET_H
