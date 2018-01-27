#include "precomp.h"
#include "EditSetView.h"

namespace Edit {

SetView::SetView(QWidget *parent)
    : QWidget(parent)
{

}

SetView::~SetView()
{

}

bool SetView::init()
{
    bool result = true;

    return result;
}

void SetView::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.save();
    QFont font = this->font();
    font.setPointSize(24);
    painter.setFont(font);
    painter.setPen(QColor(128, 128, 80, 150));
    QTextOption textOption(Qt::AlignCenter);
    painter.drawText(rect(), tr("Select left tree node to edit"), textOption);
    painter.restore();
}

}
