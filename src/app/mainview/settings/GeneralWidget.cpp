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

    comboBoxColorTheme_ = new QComboBox(groupBoxTheme);
    formLayoutTheme->addRow(tr("Color theme:"), comboBoxColorTheme_);

    vertLayoutMain->addStretch();

    comboBoxColorTheme_->addItem(tr("None"), "none");
    comboBoxColorTheme_->addItem(tr("Blue"), "blue");
    comboBoxColorTheme_->addItem(tr("Dark"), "dark");
    comboBoxColorTheme_->addItem(tr("Light"), "light");

    // initialize
    previousTheme_ = JMain::instance()->theme();
    setTheme(previousTheme_);

    connect(comboBoxColorTheme_,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](int){
#if 0   //FIXME [switch theme will crash the application, need to fix in the future]
        jnotify->post("app.theme", JMain::instance(),
                      QVariant(d_comboBoxColorTheme->currentData().toString()));
#else
        const QString theme = comboBoxColorTheme_->currentData().toString();
        if (!JMain::instance()->setConfig("global.theme.name", Json::Value(theme.toStdString()))) {
            return;
        }
        if (previousTheme_ != theme) {
            // notice
            QMessageBox::warning(this, tr("Notice"),
                                 tr("You must restart your application to apply selected theme!"));
        }
#endif
    });
}

QString GeneralWidget::theme() const
{
    return comboBoxColorTheme_->currentData().toString();
}

void GeneralWidget::setTheme(const QString &theme)
{
    for (int i = 0; i < comboBoxColorTheme_->count(); ++i) {
        if (comboBoxColorTheme_->itemData(i).toString() == theme) {
            comboBoxColorTheme_->setCurrentIndex(i);
            break;
        }
    }
}
