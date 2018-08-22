#include "precomp.h"
#include "convertfromicdwidget.h"
#include "icdwidget/JProtoTreeView.h"
#include "icdwidget/JProgressDialog.h"
#include "icdparser/icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "icdworker/icdworker_util.h"
#include <queue>

ConvertFromIcdWidget::ConvertFromIcdWidget(QWidget *parent)
    : QWidget(parent)
    , d_table(Q_NULLPTR)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayoutTop = new QFormLayout();
    vertLayoutMain->addLayout(formLayoutTop);

    d_editSource = new QLineEdit(this);
    QPushButton *buttonSourceView = new QPushButton("...");
    buttonSourceView->setFixedWidth(60);
    QHBoxLayout *horiLayoutSource = new QHBoxLayout();
    horiLayoutSource->setContentsMargins(0, 0, 0, 0);
    horiLayoutSource->addWidget(d_editSource);
    horiLayoutSource->addWidget(buttonSourceView);
    formLayoutTop->addRow(tr("Source file:"), horiLayoutSource);

    d_editTarget = new QLineEdit(this);
    QPushButton *buttonTargetView = new QPushButton("...");
    buttonTargetView->setFixedWidth(60);
    QHBoxLayout *horiLayoutTarget = new QHBoxLayout();
    horiLayoutTarget->setContentsMargins(0, 0, 0, 0);
    horiLayoutTarget->addWidget(d_editTarget);
    horiLayoutTarget->addWidget(buttonTargetView);
    formLayoutTop->addRow(tr("Target file:"), horiLayoutTarget);

    d_checkParse = new QCheckBox(" ", this);
    formLayoutTop->addRow(tr("Data Verification:"), d_checkParse);

    d_progressBar = new QProgressBar(this);
    d_progressBar->setRange(0, 100);
    vertLayoutMain->addWidget(d_progressBar);

    vertLayoutMain->addStretch();

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    d_buttonConvert = new QPushButton(tr("Convert"), this);
    d_buttonCancel = new QPushButton(tr("Cancel"), this);
    d_buttonConvert->setFixedWidth(120);
    d_buttonCancel->setFixedWidth(120);
    d_buttonConvert->setDefault(true);
    horiLayoutBottom->addWidget(d_buttonConvert);
    horiLayoutBottom->addWidget(d_buttonCancel);

    connect(buttonSourceView, &QPushButton::clicked, this, [=](){
        QString selectedFilter = tr("ICD file (*.icd)");
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Find source file"), d_editSource->text(),
                    "Any file (*.*);;"
                    "ICD file (*.icd);;"
                    "Data file (*.dat);;"
                    "Text file (*.txt)", &selectedFilter);
        if (filePath.isEmpty() || !QFile::exists(filePath)) {
            return;
        }
        //
        d_editSource->setText(filePath);
        d_editSource->setToolTip(filePath);
        //
        if (d_editTarget->text().trimmed().isEmpty()) {
            QFileInfo fileInfo(filePath);
            d_editTarget->setText(fileInfo.absolutePath().append("/")
                                  .append(fileInfo.completeBaseName()).append(".dat"));
            d_editTarget->setToolTip(d_editTarget->text());
        }
    });
    connect(buttonTargetView, &QPushButton::clicked, this, [=](){
        QString selectedFilter = tr("Dat file (*.dat)");
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Find target file"), d_editTarget->text(),
                    "Any file (*.*);;"
                    "Data file (*.dat)", &selectedFilter);
        if (filePath.isEmpty() || !QFile::exists(filePath)) {
            return;
        }
        //
        d_editTarget->setText(filePath);
        d_editTarget->setToolTip(filePath);
    });
    connect(d_buttonConvert, &QPushButton::clicked, this, [=](){
        //
        QFile sourceFile(d_editSource->text());
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            return;
        }
        // check icd header
        // [filetype:4B][attr:2B][domain:>=1B]
        if (sourceFile.size() < 7) {
            return;
        }
        int headerSize = 0;
        QString domain;
        bool hasTimeFormat = false;
        const QString header = QString(sourceFile.read(4));
        if (header != "ICD") {
            return;
        }
        headerSize += 4;
        const QByteArray attr = sourceFile.read(2);
        if (attr.size() != 2) {
            return;
        }
        headerSize += 2;
        if (attr.at(0) == 'd') {
            domain = QString::fromLocal8Bit(sourceFile.readLine(256)).trimmed();
            headerSize += domain.size() + 1;
        }
        if (attr.at(1) == 't') {
            hasTimeFormat = true;
        }
        //
        sourceFile.close();
        //
        if (!loadData(domain, headerSize, hasTimeFormat)) {
            return;
        }
    });
    connect(d_buttonCancel, &QPushButton::clicked, this, [=](){
        emit rejected();
    });
}

ConvertFromIcdWidget::~ConvertFromIcdWidget()
{

}

bool ConvertFromIcdWidget::loadData(const QString &domain, int headerSize,
                                    bool hasTimeFormat)
{
    if (headerSize < 7) {
        return false;
    }

    bool result = true;

    //
    const Icd::ParserPtr parser = JMain::instance()->parser("analyse");
    if (!parser) {
        return false;
    }

    Icd::JProgressDialog *progressDialog = new Icd::JProgressDialog(this);
    progressDialog->setWindowTitle(tr("Load data"));
    progressDialog->setProgressValue(100);
    progressDialog->setCancelVisible(false);
    progressDialog->setMessage(tr("Parsing source file..."));
    QFuture<bool> future = QtConcurrent::run([=]() -> bool {
        if (!parser->parse(domain.section('/', 0, 0).toStdString(),
                           domain.section('/', 1, 1).toStdString(),
                           domain.section('/', 2).toStdString(),
                           d_table, Icd::ObjectItem)) {
            return false;
        }
        return true;
    });
    bool parseTable = true;
    auto runAgainFuture = [=]() -> QFuture<bool> {
        return QtConcurrent::run([=]() -> bool {
            QFile sourceFile(d_editSource->text());
            QFile targetFile(d_editTarget->text());
            if (!sourceFile.open(QIODevice::ReadOnly)) {
                return false;
            }
            if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                return false;
            }
            //
            if (d_checkParse->isChecked()) {
                if (!Icd::checkData(d_table, headerSize, &sourceFile, &targetFile)) {
                    return false;
                }
            } else {
                if (hasTimeFormat) {
                    while (!sourceFile.atEnd()) {
                        sourceFile.seek(sourceFile.pos() + 8); // read time data
                        targetFile.write(sourceFile.read(int(d_table->bufferSize())));
                    }
                } else {
                    while (!sourceFile.atEnd()) {
                        targetFile.write(sourceFile.read(10240));
                    }
                }
            }
            sourceFile.close();
            targetFile.close();
            //
            return true;
        });
    };
    connect(progressDialog, &Icd::JProgressDialog::finished, this, [=,&parseTable](){
        if (progressDialog->futureResult()) {
            if (parseTable) {
                parseTable = false;
                progressDialog->setMessage(tr("Converting source file..."));
                progressDialog->setFuture(runAgainFuture());
            } else {
                progressDialog->hide();
                progressDialog->disconnect(this);
                QString message = tr("Convert success!");
                QMessageBox::information(this, tr("Converting result"), message);
                progressDialog->deleteLater();
                d_table = Icd::TablePtr();
                setEnabled(true);
            }
        } else {
            progressDialog->hide();
            progressDialog->disconnect(this);
            const QString title = parseTable ? tr("Parsing result") : tr("Converting result");
            const QString message = parseTable ? tr("Parse failure!") : tr("Convert failure!");
            QMessageBox::information(this, title, message);
            progressDialog->deleteLater();
            d_table = Icd::TablePtr();
        }
    });
    progressDialog->setFuture(future);
    progressDialog->exec();

    return result;
}
