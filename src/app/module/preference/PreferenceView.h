#ifndef PREFERENCEVIEW_H
#define PREFERENCEVIEW_H

#include <QWidget>

class JSplitter;
class QListWidget;
class QScrollArea;

namespace Preference {

class View : public QWidget
{
    Q_OBJECT
public:
    enum ItemType {
        ItemTypeInvalid = -1,
        ItemTypeChannel,
        ItemTypeDataSource
    };

    explicit View(QWidget *parent = nullptr);
    ~View();

    bool init();

signals:

public slots:
    void onCurrentRowChanged(int currentRow);

private:
    JSplitter *d_splitter;
    QListWidget *d_listWidget;
    QScrollArea *d_scrollArea;
};

}

#endif // PREFERENCEVIEW_H
