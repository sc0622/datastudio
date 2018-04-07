#ifndef EDITSETVIEW_H
#define EDITSETVIEW_H

#include <QWidget>

class QStackedWidget;
class QStandardItem;

namespace Edit {

class DetailView;

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

private:
    void onTreeCurrentChanged(QStandardItem *current, QStandardItem *previous);

private:
    QStackedWidget *d_stackedWidget;
    DetailView *d_detailView;
};

}

#endif // EDITSETVIEW_H
