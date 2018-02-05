#ifndef GENERALWIDGET_H
#define GENERALWIDGET_H

#include <QWidget>

class QComboBox;

class GeneralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralWidget(QWidget *parent = nullptr);

    QString theme() const;

signals:

public slots:
    void setTheme(const QString &theme);

private:
    QComboBox *d_comboBoxColorTheme;
};

#endif // GENERALWIDGET_H
