#include "precomp.h"
#include "TableEdit.h"

namespace Edit {

// class TableEdit

TableEdit::TableEdit(const Icd::TablePtr &table, QWidget *parent)
    : ObjectEdit(table, parent)
{

}

TableEdit::~TableEdit()
{

}

Icd::TablePtr TableEdit::table() const
{
    return JHandlePtrCast<Icd::Table>(object());
}

}
