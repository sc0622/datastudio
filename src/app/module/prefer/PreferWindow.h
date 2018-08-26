﻿#ifndef PREFERWINDOW_H
#define PREFERWINDOW_H

#include <QWidget>

class JSplitter;
class QListWidget;
class QScrollArea;

namespace Prefer {

class Window : public QWidget
{
    Q_OBJECT
public:
    enum ItemType {
        ItemTypeInvalid = -1,
        ItemTypeChannel,
        ItemTypeDataSource
    };

    explicit Window(QWidget *parent = nullptr);
    ~Window();

    bool init();

signals:

public slots:
    void onCurrentRowChanged(int currentRow);

private:
    bool loadChannelConfig();

private:
    JSplitter *splitter_;
    QListWidget *listWidget_;
    QScrollArea *scrollArea_;
};

}

#endif // PREFERWINDOW_H
