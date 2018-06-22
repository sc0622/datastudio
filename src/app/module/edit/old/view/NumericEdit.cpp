#include "precomp.h"
#include "NumericEdit.h"

NumericEdit::NumericEdit(QWidget *parent)
    : ItemEdit(wdNumeric, parent)
{

}

int NumericEdit::windowType() const
{
    return wdNumeric;
}
