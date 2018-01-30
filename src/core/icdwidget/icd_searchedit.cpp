#include "precomp.h"
#include "icd_searchedit.h"

namespace Icd {

// class SearchEditPrivate

class SearchEditPrivate
{
public:
    SearchEditPrivate(SearchEdit *q)
        : q_ptr(q)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(SearchEdit)
    QLineEdit *lineEdit;
};

void SearchEditPrivate::init()
{
    Q_Q(SearchEdit);
    q->setObjectName("SearchEdit");
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
    QObject::connect(lineEdit, &QLineEdit::textChanged, q, &SearchEdit::textChanged);
}

// class SearchEdit

SearchEdit::SearchEdit(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SearchEditPrivate(this))
{
    Q_D(SearchEdit);
    d->init();
}

SearchEdit::~SearchEdit()
{
    Q_D(SearchEdit);
    delete d;
}

} // end of namespace Icd
