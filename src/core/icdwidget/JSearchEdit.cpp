#include "precomp.h"
#include "JSearchEdit.h"

namespace Icd {

// class JSearchEditPrivate

class JSearchEditPrivate
{
public:
    JSearchEditPrivate(JSearchEdit *q)
        : J_QPTR(q)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JSearchEdit)
    QLineEdit *lineEdit;
};

void JSearchEditPrivate::init()
{
    Q_Q(JSearchEdit);
    q->setObjectName("JSearchEdit");
    q->setFixedHeight(24);

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(q);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);
    horiLayoutMain->setSpacing(2);

    QLabel *labelIcon = new QLabel(q);
    labelIcon->setFixedSize(q->height(), q->height());
    labelIcon->setPixmap(QPixmap(":/icdwidget/image/search.png")
                         .scaled(labelIcon->size(), Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation));
    horiLayoutMain->addWidget(labelIcon);

    lineEdit = new QLineEdit(q);
    lineEdit->setFixedHeight(q->height());
    horiLayoutMain->addWidget(lineEdit);

    //
    QObject::connect(lineEdit, &QLineEdit::textChanged, q, &JSearchEdit::textChanged);
}

// class JSearchEdit

JSearchEdit::JSearchEdit(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new JSearchEditPrivate(this))
{
    Q_D(JSearchEdit);
    d->init();
}

JSearchEdit::~JSearchEdit()
{
    Q_D(JSearchEdit);
    delete d;
}

} // end of namespace Icd
