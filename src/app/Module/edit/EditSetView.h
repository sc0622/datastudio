#ifndef EDITSETVIEW_H
#define EDITSETVIEW_H

#include <QWidget>

class QStandardItem;

namespace Edit {

class SetView : public QWidget
{
    Q_OBJECT
public:
    explicit SetView(QWidget *parent = nullptr);
    ~SetView();

    bool init();

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    void onTreeCurrentChanged(QStandardItem *current, QStandardItem *previous);

private:
};

}

#endif // EDITSETVIEW_H
