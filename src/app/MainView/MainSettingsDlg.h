#ifndef MAINSETTINGSDLG_H
#define MAINSETTINGSDLG_H

#include <QDialog>

class JSplitter;
class JTreeView;
class QScrollArea;
class QStandardItem;

namespace Main {

class SettingsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDlg(QWidget *parent = nullptr);
    ~SettingsDlg();

    bool init();

signals:

public slots:
    void onCurrentItemChanged(QStandardItem *current, QStandardItem *previous);

private:
    JSplitter *d_splitter;
    JTreeView *d_treeView;
    QScrollArea *d_scrollArea;
};

}

#endif // MAINSETTINGSDLG_H
