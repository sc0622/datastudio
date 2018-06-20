#include "precomp.h"
#include "numericui.h"

NumericUI::NumericUI(QWidget *parent)
    : CommonUI(wdNumeric, parent)
{

}

int NumericUI::uiType() const
{
    return wdNumeric;
}
