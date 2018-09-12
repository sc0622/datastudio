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
    void onTreeCurrentChanged(QStandardItem *current, QStandardItem *previous);
    void onEditTriggered(QStandardItem *item, int editAction, const QVariant &data);

protected:

private:
    QStackedWidget *stackedWidget_;
    DetailView *detailView_;
};

}

#endif // EDITSETVIEW_H
