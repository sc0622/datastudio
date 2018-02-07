#include "precomp.h"
#include "DetailItemEdit.h"

namespace Edit {

DetailItemEdit::DetailItemEdit(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_groupBox = new QGroupBox("test", this);
    vertLayoutMain->addWidget(d_groupBox);
}

}
