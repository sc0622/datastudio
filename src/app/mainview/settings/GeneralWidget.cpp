#include "precomp.h"
#include "GeneralWidget.h"

GeneralWidget::GeneralWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(6, 6, 6, 6);

    QGroupBox *groupBoxTheme = new QGroupBox(tr("Visual experience"), this);
    vertLayoutMain->addWidget(groupBoxTheme);

    QFormLayout *formLayoutTheme = new QFormLayout(groupBoxTheme);

    d_comboBoxColorTheme = new QComboBox(groupBoxTheme);
    formLayoutTheme->addRow(tr("Color theme:"), d_comboBoxColorTheme);

    vertLayoutMain->addStretch();

    d_comboBoxColorTheme->addItem(tr("None"), "none");
    d_comboBoxColorTheme->addItem(tr("Blue"), "blue");
    d_comboBoxColorTheme->addItem(tr("Dark"), "dark");
    d_comboBoxColorTheme->addItem(tr("Light"), "light");

    connect(d_comboBoxColorTheme,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](int){
        JMain::instance()->setTheme(d_comboBoxColorTheme->currentData().toString());
    });

    // initialize
    setTheme(JMain::instance()->theme());
}

QString GeneralWidget::theme() const
{
    return d_comboBoxColorTheme->currentData().toString();
}

void GeneralWidget::setTheme(const QString &theme)
{
    for (int i = 0; i < d_comboBoxColorTheme->count(); ++i) {
        if (d_comboBoxColorTheme->itemData(i).toString() == theme) {
            d_comboBoxColorTheme->setCurrentIndex(i);
            break;
        }
    }
}
