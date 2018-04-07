#include "precomp.h"
#include "PreferenceView.h"
#include "channel/ChannelView.h"
#include "datasource/DataSourceView.h"

namespace Preference {

View::View(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_splitter = new JSplitter(this);
    d_splitter->setObjectName("main.preference.view.splitter");
    d_splitter->setHandleWidth(5);
    d_splitter->setScales(QList<double>() << 1 << 5);
    vertLayoutMain->addWidget(d_splitter);

    d_listWidget = new QListWidget(this);
    d_listWidget->setFrameShape(QFrame::NoFrame);
    d_listWidget->setViewMode(QListView::IconMode);
    d_listWidget->setResizeMode(QListView::Adjust);
    d_listWidget->setUniformItemSizes(true);
    d_splitter->addWidget(d_listWidget);

    d_scrollArea = new QScrollArea(this);
    d_scrollArea->setFrameShape(QFrame::NoFrame);
    d_scrollArea->setWidgetResizable(true);
    d_splitter->addWidget(d_scrollArea);

    connect(d_listWidget, &QListWidget::currentRowChanged, this, &View::onCurrentRowChanged);
}

View::~View()
{
    JMain::saveWidgetState(d_splitter);
}

bool View::init()
{
    bool result = true;

    if (result) {
        JMain::restoreWidgetState(d_splitter);
    }

    d_listWidget->clear();

    // channel item
    QListWidgetItem *itemChannel = new QListWidgetItem(
                QIcon(":/datastudio/image/global/channel.png"),
                tr("Channel management"));
    d_listWidget->addItem(itemChannel);

    // datasource item
    QListWidgetItem *itemDataSource = new QListWidgetItem(
                QIcon(":/datastudio/image/toolbar/database.png"),
                tr("Data source management"));
    d_listWidget->addItem(itemDataSource);

    // test
    QListWidgetItem *itemDataSource2 = new QListWidgetItem(
                QIcon(":/datastudio/image/toolbar/database.png"),
                tr("item #1"));
    d_listWidget->addItem(itemDataSource2);

    // set current type
    d_listWidget->setCurrentRow(ItemTypeChannel);

    return result;
}

void View::onCurrentRowChanged(int currentRow)
{
    QWidget *widget = d_scrollArea->takeWidget();
    if (widget) {
        widget->deleteLater();
    }

    if (currentRow == -1) {
        return;
    }

    switch (currentRow) {
    case ItemTypeChannel:
    {
        ChannelView *channelView = new ChannelView(d_scrollArea);
        d_scrollArea->setWidget(channelView);
        break;
    }
    case ItemTypeDataSource:
    {
        DataSourceView *dataSourceView = new DataSourceView(d_scrollArea);
        d_scrollArea->setWidget(dataSourceView);
        break;
    }
    default:
        break;
    }
}

}
