#include "precomp.h"
#include "MainSettingsDlg.h"

namespace Main {

SettingsDlg::SettingsDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Global Settings"));
    resize(700, 300);
}

SettingsDlg::~SettingsDlg()
{

}

}
