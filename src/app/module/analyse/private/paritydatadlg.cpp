#include "precomp.h"
#include "paritydatadlg.h"
#include "icdwidget/JProtoTreeView.h"
#include "icdwidget/JProgressDialog.h"
#include "icdparser/icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "icdworker/icdworker_util.h"
#include "../tablesel_widget.h"

ParityDataDlg::ParityDataDlg(QWidget *parent)
    : QWidget(parent)
    , d_table(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayoutTop = new QFormLayout();
    formLayoutTop->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
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

    d_progressBar = new QProgressBar(this);
    d_progressBar->setRange(0, 100);
    vertLayoutMain->addWidget(d_progressBar);

    vertLayoutMain->addStretch();

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    d_buttonConvert = new QPushButton(tr("Start converting"), this);
    d_buttonCancel = new QPushButton(tr("Cancel"), this);
    d_buttonConvert->setFixedWidth(120);
    d_buttonCancel->setFixedWidth(120);
    d_buttonConvert->setDefault(true);
    horiLayoutBottom->addWidget(d_buttonConvert);
    horiLayoutBottom->addWidget(d_buttonCancel);

    connect(buttonSourceView, &QPushButton::clicked, this, [=](){
        QString selectedFilter = QString("Any file (*.*)");
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select source file"), d_editSource->text(),
                    "Any file (*.*);;"
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
        QString selectedFilter = QString("Dat file (*.dat)");
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Select target file"), d_editTarget->text(),
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
        bool hasTimeFormat = false;
        QString domain;

        // 判断文件类型
        const QString header = QString(sourceFile.read(4));
        if (header == "ICD") {
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
        }

        if (domain.isEmpty()) {
            TableSelWidget tableSelWidget(this);
            tableSelWidget.resize(600, 660);
            if (!tableSelWidget.init()) {
                return;
            }
            if (tableSelWidget.exec() != QDialog::Accepted) {
                return;
            }
            domain = tableSelWidget.tableDomain();
            if (domain.isEmpty()) {
                return;
            }
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

ParityDataDlg::~ParityDataDlg()
{

}

bool ParityDataDlg::loadData(const QString &domain, int headerSize,
                             bool hasTimeFormat)
{
    Q_UNUSED(hasTimeFormat);
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
    progressDialog->setMessage(tr("Parsing data file..."));
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
            const QString sourcePath = d_editSource->text().trimmed();
            const QString targetPath = [=]() -> QString {
                QString path = d_editTarget->text().trimmed();
                if (sourcePath == path) {
                    path.append(".bak");
                }
                return path;
            }();
            QFile sourceFile(sourcePath);
            if (!sourceFile.open(QIODevice::ReadOnly)) {
                return false;
            }
            QFile targetFile(targetPath);
            if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                return false;
            }
            //
            if (!Icd::checkData(d_table, headerSize, &sourceFile, &targetFile)) {
                targetFile.close();
                targetFile.remove();
                return false;
            }
            //
            sourceFile.close();
            targetFile.close();
            if (targetPath.endsWith(".bak")) {
                if (!sourceFile.remove()) {
                    qDebug() << "source:" << sourceFile.errorString();
                }
                if (!targetFile.rename(sourcePath)) {
                    qDebug() << "target:" << targetFile.errorString();
                }
            }
            //
            return true;
        });
    };
    connect(progressDialog, &Icd::JProgressDialog::finished, this, [=,&parseTable](){
        if (progressDialog->futureResult()) {
            if (parseTable) {
                parseTable = false;
                progressDialog->setMessage(tr("Converting data file..."));
                progressDialog->setFuture(runAgainFuture());
            } else {
                progressDialog->hide();
                progressDialog->disconnect(this);
                QString message = tr("Convert success!");
                QMessageBox::information(this, tr("Result of converting"), message);
                progressDialog->deleteLater();
                d_table = Icd::TablePtr();
                setEnabled(true);
            }
        } else {
            progressDialog->hide();
            progressDialog->disconnect(this);
            const QString title = parseTable ? tr("Result of parsing") : tr("Result of converting");
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
