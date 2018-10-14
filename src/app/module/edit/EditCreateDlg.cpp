#include "precomp.h"
#include "EditCreateDlg.h"

namespace Edit {

// class EditCreateDlg

EditCreateDlg::EditCreateDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Create data source"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayoutTop = new QFormLayout();
    formLayoutTop->setLabelAlignment(Qt::AlignRight);
    formLayoutTop->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    vertLayoutMain->addLayout(formLayoutTop);

    QHBoxLayout *horiLayoutFilePath = new QHBoxLayout();
    horiLayoutFilePath->setSpacing(0);
    formLayoutTop->addRow(tr("File path:"), horiLayoutFilePath);

    editFilePath_ = new QLineEdit(this);
    horiLayoutFilePath->addWidget(editFilePath_);

    QPushButton *buttonView = new QPushButton(QIcon(":/datastudio/image/global/view.png"),
                                              QString(), this);
    buttonView->setFixedSize(35, editFilePath_->minimumSizeHint().height());
    horiLayoutFilePath->addWidget(buttonView);

    vertLayoutMain->addSpacing(20);

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    QPushButton *buttonOk = new QPushButton(tr("Ok"), this);
    buttonOk->setMinimumWidth(80);
    horiLayoutBottom->addWidget(buttonOk);

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonCancel->setMinimumWidth(80);
    horiLayoutBottom->addWidget(buttonCancel);

    resize(500, sizeHint().height());

    connect(buttonView, &QPushButton::clicked, this, [=](){
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Select file"), editFilePath_->text(),
                    "JSON file (*.json);;XML file (*.xml)").trimmed();
        if (filePath.isEmpty()) {
            return;
        }
        editFilePath_->setText(filePath);
    });
    connect(buttonOk, &QPushButton::clicked, this, [=](){
        if (!saveConfig()) {
            return;
        }
        this->accept();
    });
    connect(buttonCancel, &QPushButton::clicked, this, [=](){
        this->reject();
    });

    //
    editFilePath_->setText(QStandardPaths::locate(QStandardPaths::DesktopLocation,
                                                  QString(),  QStandardPaths::LocateDirectory)
                           + "untitled1.json");
}

QString EditCreateDlg::filePath() const
{
    return editFilePath_->text().trimmed();
}

void EditCreateDlg::focusFilePath()
{
    editFilePath_->setFocus();
    editFilePath_->selectAll();
}

void EditCreateDlg::setFilePath(const QString &filePath)
{
    editFilePath_->setText(filePath);
}

bool EditCreateDlg::saveConfig()
{
    // save config
    Json::Value config;
    // source type
    config["sourceType"] = "file";
    // file path
    const QString filePath = this->filePath();
    if (filePath.isEmpty()) {
        focusFilePath();
        return false;
    }

    if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        if (!Json::make(filePath.toLocal8Bit().data(), Json::Value(Json::objectValue), true, true)) {
            return false;
        }
    } else if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        if (!Icd::Parser::createXmlFile(filePath.toLocal8Bit().data())) {
            return false;
        }
    } else {
        return false;
    }

    if (!QFile::exists(filePath)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("File \"%1\" is not exists!").arg(filePath));
        focusFilePath();
        return false;
    }

    config["filePath"] = filePath.toStdString();
    if (!JMain::instance()->setOption("edit", "parser", config)) {
        return false;
    }

    return true;
}

}
