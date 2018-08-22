#include "precomp.h"
#include "PreferWindow.h"
#include "channel/ChannelView.h"
#include "datasource/DataSourceView.h"

namespace Prefer {

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    splitter_ = new JSplitter(this);
    splitter_->setObjectName("prefer.window.splitter.main");
    splitter_->setOpaqueResize(true);
    splitter_->setHandleWidth(5);
    splitter_->setScales(QList<double>() << 1 << 6);
    vertLayoutMain->addWidget(splitter_);

    listWidget_ = new QListWidget(this);
    listWidget_->setFrameShape(QFrame::NoFrame);
    listWidget_->setViewMode(QListView::ListMode);
    QFont font = listWidget_->font();
    font.setPointSize(font.pointSize() + 2);
    listWidget_->setFont(font);
    listWidget_->setIconSize(QSize(30, 30));
    splitter_->addWidget(listWidget_);

    scrollArea_ = new QScrollArea(this);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setWidgetResizable(true);
    splitter_->addWidget(scrollArea_);

    connect(listWidget_, &QListWidget::currentRowChanged,
            this, &Window::onCurrentRowChanged);
}

Window::~Window()
{
    JMain::saveWidgetState(splitter_);
    JMain::saveWidgetState(listWidget_);
}

bool Window::init()
{
    bool result = true;

    if (result) {
        JMain::restoreWidgetState(splitter_);
    }

    listWidget_->clear();

    // channel item
    QListWidgetItem *itemChannel = new QListWidgetItem(
                QIcon(":/datastudio/image/global/channel.png"),
                tr("Channel management"));
    itemChannel->setSizeHint(QSize(itemChannel->sizeHint().width(), 40));
    listWidget_->addItem(itemChannel);
    // database item
    QListWidgetItem *itemDatabase = new QListWidgetItem(
                QIcon(":/datastudio/image/toolbar/database.png"),
                tr("Database management"));
    itemDatabase->setSizeHint(QSize(itemChannel->sizeHint().width(), 40));
    listWidget_->addItem(itemDatabase);

    if (result) {
        JMain::restoreWidgetState(listWidget_);
    }

    return result;
}

void Window::onCurrentRowChanged(int currentRow)
{
    QWidget *widget = scrollArea_->takeWidget();
    if (widget) {
        widget->deleteLater();
    }

    if (currentRow == -1) {
        return;
    }

    switch (currentRow) {
    case ItemTypeChannel:
    {
        ChannelView *channelView = new ChannelView(scrollArea_);
        scrollArea_->setWidget(channelView);
        break;
    }
    case ItemTypeDataSource:
    {
        DataSourceView *dataSourceView = new DataSourceView(scrollArea_);
        scrollArea_->setWidget(dataSourceView);
        break;
    }
    default:
        break;
    }
}

}
