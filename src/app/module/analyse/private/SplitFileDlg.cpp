#include "precomp.h"
#include "SplitFileDlg.h"
#include "icdwidget/JProtoTreeView.h"
#include "icdwidget/JProgressDialog.h"
#include "icdparser/icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "icdworker/icdworker_util.h"
#include "../tablesel_widget.h"

SplitFileDlg::SplitFileDlg(QWidget *parent)
    : QWidget(parent)
    , d_table(Q_NULLPTR)
    , d_progressDialog(Q_NULLPTR)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayoutTop = new QFormLayout();
    formLayoutTop->setLabelAlignment(Qt::AlignRight);
    formLayoutTop->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    vertLayoutMain->addLayout(formLayoutTop);

    d_comboFileType = new QComboBox(this);
    d_comboFileType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d_comboFileType->addItem(tr("Pure data file"));
    d_comboFileType->addItem(tr("ICD data file"));
    d_comboFileType->addItem(tr("Text file (ATX,ATXEX)"));
    d_comboFileType->addItem(tr("Universal binary file (GBR)"));
    d_comboFileType->addItem(tr("Universal original data file (RAW)"));
    d_comboFileType->addItem(tr("RASS realtime data file (MRD)"));
    formLayoutTop->addRow(tr("File type:"), d_comboFileType);

    d_comboSplitSize = new QComboBox(this);
    d_comboSplitSize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d_comboSplitSize->addItem("50 MB", 50);
    d_comboSplitSize->addItem("100 MB", 100);
    d_comboSplitSize->addItem("200 MB", 200);
    d_comboSplitSize->addItem("256 MB", 256);
    d_comboSplitSize->addItem("400 MB", 400);
    d_comboSplitSize->addItem("512 MB", 512);
    d_comboSplitSize->addItem("700 MB", 700);
    d_comboSplitSize->addItem("1024 MB", 1024);
    formLayoutTop->addRow(tr("Volume size:"), d_comboSplitSize);

    d_editSource = new QLineEdit();
    QPushButton *buttonSourceView = new QPushButton("...");
    buttonSourceView->setFixedWidth(60);
    QHBoxLayout *horiLayoutSource = new QHBoxLayout();
    horiLayoutSource->setContentsMargins(0, 0, 0, 0);
    horiLayoutSource->addWidget(d_editSource);
    horiLayoutSource->addWidget(buttonSourceView);
    formLayoutTop->addRow(tr("Source file:"), horiLayoutSource);

    d_editTarget = new QLineEdit();
    QPushButton *buttonTargetView = new QPushButton("...");
    buttonTargetView->setFixedWidth(60);
    QHBoxLayout *horiLayoutTarget = new QHBoxLayout();
    horiLayoutTarget->setContentsMargins(0, 0, 0, 0);
    horiLayoutTarget->addWidget(d_editTarget);
    horiLayoutTarget->addWidget(buttonTargetView);
    formLayoutTop->addRow(tr("Target file:"), horiLayoutTarget);

    d_checkAsFTI = new QCheckBox(tr("Split file when system counter is equal to zero"), this);
    //d_checkAsFTI->setChecked(false);
    formLayoutTop->addRow(tr("Split file with FTI protocol:"), d_checkAsFTI);

    vertLayoutMain->addStretch();

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    d_buttonSplit = new QPushButton(tr("Start split"), this);
    d_buttonCancel = new QPushButton(tr("Cancel"), this);
    d_buttonSplit->setFixedWidth(120);
    d_buttonCancel->setFixedWidth(120);
    d_buttonSplit->setDefault(true);
    horiLayoutBottom->addWidget(d_buttonSplit);
    horiLayoutBottom->addWidget(d_buttonCancel);

    connect(d_comboFileType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](int index){
        switch (index) {
        case 0:
        case 1:
        case 2:
            d_buttonSplit->setEnabled(true);
            break;
        case -1:
        default:
            d_buttonSplit->setEnabled(false);
            break;
        }
    });
    connect(buttonSourceView, &QPushButton::clicked, this, [=](){
        QStringList filters;
        filters << "Any file (*.*)" << "Data file (*.dat)"
                << "Text file (*.icd)" << "Text file (*.atx)";
        QString selectedFilter = filters.first();
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select source file"), d_editSource->text(),
                    filters.join(";;"), &selectedFilter);
        if (filePath.isEmpty() || !QFile::exists(filePath)) {
            return;
        }
        //
        d_editSource->setText(filePath);
        d_editSource->setToolTip(filePath);
        //
        int index = filters.indexOf(selectedFilter);
        switch (index) {
        case 0:
        {
            QFileInfo fileInfo(filePath);
            const QString suffix = fileInfo.suffix().toLower();
            if (suffix == "icd") {
                d_comboFileType->setCurrentIndex(1);
            } else if (suffix == "atx") {
                d_comboFileType->setCurrentIndex(2);
            } else if (suffix == "gbr") {
                d_comboFileType->setCurrentIndex(3);
            } else if (suffix == "raw") {
                d_comboFileType->setCurrentIndex(4);
            } else if (suffix == "mrd") {
                d_comboFileType->setCurrentIndex(5);
            }
            break;
        }
        case 1:
            d_comboFileType->setCurrentIndex(0);
            break;
        case 2:
            d_comboFileType->setCurrentIndex(1);
            break;
        case 3:
            d_comboFileType->setCurrentIndex(2);
            break;
        default:
            break;
        }
        //
        if (d_editTarget->text().trimmed().isEmpty()) {
            QFileInfo fileInfo(filePath);
            d_editTarget->setText(fileInfo.absolutePath());
            d_editTarget->setToolTip(d_editTarget->text());
        }
    });
    connect(buttonTargetView, &QPushButton::clicked, this, [=](){
        const QString filePath = QFileDialog::getExistingDirectory(
                    this, tr("Select target file"), d_editTarget->text());
        if (filePath.isEmpty() || !QFile::exists(filePath)) {
            return;
        }
        //
        d_editTarget->setText(filePath);
        d_editTarget->setToolTip(filePath);
    });
    connect(d_buttonSplit, &QPushButton::clicked, this, [=](){
        const QString filePath = d_editSource->text().trimmed();
        if (filePath.isEmpty()) {
            d_editSource->setFocus();
            d_editSource->selectAll();
            return;
        }
        if (QFileInfo(filePath).size() <= d_comboSplitSize->itemData(0).toInt() * 1024L * 1024L) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Size of source file is less than 50MB, not need to split!"));
            return;
        }
        const QString targetDir = d_editTarget->text().trimmed();
        if (targetDir.isEmpty()) {
            d_editTarget->setFocus();
            d_editTarget->selectAll();
            return;
        }

        bool result = false;
        const qint64 splitSize = d_comboSplitSize->currentData().toInt() * 1024L * 1024L;
        switch (d_comboFileType->currentIndex()) {
        case 0:
            result = splitDataFile(filePath, targetDir, splitSize);
            break;
        case 1:
            result = splitIcdFile(filePath, targetDir, splitSize);
            break;
        case 2:
            result = splitATXFile(filePath, targetDir, splitSize);
            break;
        case 3:
            result = splitGBRFile(filePath, targetDir, splitSize);
            break;
        case 4:
            result = splitRAWFile(filePath, targetDir, splitSize);
            break;
        case 5:
            result = splitMRDFile(filePath, targetDir, splitSize);
            break;
        default:
            break;
        }

        if (!result) {
            //
            return;
        }
    });
    connect(d_buttonCancel, &QPushButton::clicked, this, [=](){
        emit rejected();
    });

    d_comboSplitSize->setCurrentIndex(3);   // default: 256 MB
}

bool SplitFileDlg::splitDataFile(const QString &filePath, const QString &targetDir,
                                 qint64 splitSize)
{
    //
    QFile sourceFile(filePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!QDir(targetDir).exists()) {
        return false;
    }

    TableSelWidget tableSelWidget(this);
    tableSelWidget.resize(600, 660);
    if (!tableSelWidget.init()) {
        return false;
    }

    if (tableSelWidget.exec() != QDialog::Accepted) {
        return false;
    }

    const QString domain = tableSelWidget.tableDomain();
    if (domain.isEmpty()) {
        return false;
    }

    sourceFile.close();

    if (!loadTable(filePath, targetDir, splitSize, domain, 0, false)) {
        return false;
    }

    return true;
}

bool SplitFileDlg::splitIcdFile(const QString &filePath, const QString &targetDir,
                                qint64 splitSize)
{
    //
    QFile sourceFile(filePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!QDir(targetDir).exists()) {
        return false;
    }

    // check icd header
    // [filetype:4B][attr:2B][domain:>=1B]
    if (sourceFile.size() < 7) {
        return false;
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
            return false;
        }
        headerSize += 2;
        if (attr.at(0) == 'd') {
            domain = QString::fromLocal8Bit(sourceFile.readLine(256)).trimmed();
            headerSize += domain.size() + 1;
        }
        if (attr.at(1) == 't') {
            hasTimeFormat = true;
        }
    } else {
        return false;
    }

    if (domain.isEmpty()) {
        return false;
    }

    sourceFile.close();

    if (!loadTable(filePath, targetDir, splitSize, domain,
                   headerSize, hasTimeFormat)) {
        return false;
    }

    return true;
}

bool SplitFileDlg::splitATXFile(const QString &filePath, const QString &targetDir,
                                qint64 splitSize)
{
    Q_UNUSED(targetDir);
    bool result = true;

    if (d_progressDialog) {
        d_progressDialog->disconnect();
        d_progressDialog->deleteLater();
        d_progressDialog = Q_NULLPTR;
    }

    d_progressDialog = new Icd::JProgressDialog(this);
    d_progressDialog->setCancelVisible(false);
    d_progressDialog->setInvertedAppearance(false);
    d_progressDialog->setWindowTitle(tr("Split file"));
    d_progressDialog->setMessage(tr("Splitting file..."));
    connect(this, &SplitFileDlg::progressMessageChanged,
            this, [=](const QString &message){
        if (d_progressDialog) {
            d_progressDialog->setMessage(message);
        }
    });
    connect(this, &SplitFileDlg::progressRangeChanged,
            this, [=](int minimum, int maximum){
        if (d_progressDialog) {
            d_progressDialog->setProgressRange(minimum, maximum);
        }
    });
    connect(this, &SplitFileDlg::progressValueChanged, this, [=](int value){
        if (d_progressDialog) {
            d_progressDialog->setProgressValue(value);
        }
    });

    JAutoCursor busyCursor(Qt::BusyCursor);

    QFuture<bool> future = QtConcurrent::run([=]() -> bool {
        QFile sourceFile(filePath);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            return false;
        }

        if (sourceFile.atEnd()) {
            return false;
        }

        bool result = true;
        int fileIndex = 1;
        const QFileInfo fileInfo(sourceFile.fileName());
        QStringList targetFiles;
        QByteArray headerContents, line;
        qint64 pos = 0;
        int indexOfCounter = -1;
        QList<QByteArray> splitLine;
        qint64 packageIndex = 0;
        // comment
        headerContents.append(sourceFile.readLine(1024 * 10));
        if (sourceFile.atEnd()) {
            return false;
        }
        // count
        headerContents.append(sourceFile.readLine(50));
        if (sourceFile.atEnd()) {
            return false;
        }
        // variables
        if (d_checkAsFTI->isChecked()) {
            line = sourceFile.readLine();
            if (line.isEmpty()) {
                return false;
            }
            //
            headerContents.append(line);
            if (sourceFile.atEnd()) {
                return false;
            }
            //
            splitLine = line.split(' ');
            line.clear();
            const QString msg = tr("System counter");
            indexOfCounter = splitLine.indexOf(msg.toLocal8Bit());
        } else {
            headerContents.append(sourceFile.readLine());
        }

        emit progressRangeChanged(0, int(sourceFile.size() / 1024));
        emit progressValueChanged(0);

        const QString baseName = fileInfo.completeBaseName();
        const QString splitPath = fileInfo.absolutePath() + '/' + baseName + "-sp-"
                + QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmsszzz");
        QDir splitDir(splitPath);
        if (!splitDir.exists()) {
            if (!splitDir.mkpath(splitPath)) {
                return false;
            }
        }

        while (!sourceFile.atEnd()) {
            qint64 packageCount = 0;
            targetFiles.append(splitPath + QString("/%1#(%2).%3")
                               .arg(baseName)
                               .arg(fileIndex)
                               .arg(fileInfo.suffix()));
            QSaveFile targetFile(targetFiles.last());
            if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
                result = false;
                break;
            }

            // header
            targetFile.write(headerContents);
            //
            do {
                line = sourceFile.readLine();
                if (indexOfCounter >= 0) {
                    pos = sourceFile.pos();
                    splitLine = line.split(' ');
                    if (indexOfCounter < splitLine.count()) {
                        if (splitLine.at(indexOfCounter).toUInt() == 0) {
                            line.clear();
                            sourceFile.seek(pos);
                            break;
                        }
                    }
                }
                ++packageCount;
                targetFile.write(line);
                emit progressValueChanged(int(sourceFile.pos() / 1024));
            } while (!sourceFile.atEnd() && targetFile.size() < splitSize);

            if (!result) {
                break;
            }

            if (!targetFile.commit()) {
                result = false;
                break;
            }

            // rename file
            const QString newFilePath = splitPath + QString("/%1#(%2)-(%3~%4).%5")
                    .arg(baseName)
                    .arg(fileIndex)
                    .arg(packageIndex + 1)
                    .arg(packageIndex + packageCount)
                    .arg(fileInfo.suffix());
            targetFiles.last() = newFilePath;
            QFile::rename(targetFile.fileName(), newFilePath);
            packageIndex += packageCount;

            //
            ++fileIndex;

            //
            emit progressValueChanged(int((sourceFile.pos() / 1024)));
        }

        if (!result) {
            foreach (auto &file, targetFiles) {
                QFile::remove(file);
            }
        }

        targetFiles.clear();

        return result;
    });
    connect(d_progressDialog, &Icd::JProgressDialog::finished, this, [=](){
        d_progressDialog->hide();
        d_progressDialog->disconnect(this);
        QString message =  d_progressDialog->futureResult()
                ? tr("Split success!")
                : tr("Split failure!");
        QMessageBox::information(this,  tr("Result of split"), message);
        d_progressDialog->deleteLater();
        d_progressDialog = Q_NULLPTR;
        d_table = Icd::TablePtr();
    });
    d_progressDialog->setFuture(future);
    d_progressDialog->exec();
    d_progressDialog->disconnect(this);

    return result;
}

bool SplitFileDlg::splitGBRFile(const QString &filePath, const QString &targetDir,
                                qint64 splitSize)
{
    //
    QFile sourceFile(filePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!QDir(targetDir).exists()) {
        return false;
    }

    Q_UNUSED(splitSize);
    return false;
}

bool SplitFileDlg::splitRAWFile(const QString &filePath, const QString &targetDir,
                                qint64 splitSize)
{
    //
    QFile sourceFile(filePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!QDir(targetDir).exists()) {
        return false;
    }

    Q_UNUSED(splitSize);
    return false;
}

bool SplitFileDlg::splitMRDFile(const QString &filePath, const QString &targetDir,
                                qint64 splitSize)
{
    //
    QFile sourceFile(filePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!QDir(targetDir).exists()) {
        return false;
    }

    Q_UNUSED(splitSize);
    return false;
}

bool SplitFileDlg::loadTable(const QString &filePath, const QString &targetDir,
                             qint64 splitSize, const QString &domain, int headerSize,
                             bool hasTimeFormat)
{
    bool result = true;

    //
    const Icd::ParserPtr parser = JMain::instance()->parser("analyse");
    if (!parser) {
        return false;
    }

    JAutoCursor busyCursor(Qt::BusyCursor);

    if (d_progressDialog) {
        d_progressDialog->disconnect();
        d_progressDialog->deleteLater();
        d_progressDialog = Q_NULLPTR;
    }

    d_progressDialog = new Icd::JProgressDialog(this);
    d_progressDialog->setWindowTitle(tr("Load data"));
    d_progressDialog->setCancelVisible(false);
    d_progressDialog->setInvertedAppearance(false);
    d_progressDialog->setMessage(tr("Parsing file..."));
    connect(this, &SplitFileDlg::progressMessageChanged,
            this, [=](const QString &message){
        if (d_progressDialog) {
            d_progressDialog->setMessage(message);
        }
    });
    connect(this, &SplitFileDlg::progressRangeChanged,
            this, [=](int minimum, int maximum){
        if (d_progressDialog) {
            d_progressDialog->setProgressRange(minimum, maximum);
        }
    });
    connect(this, &SplitFileDlg::progressValueChanged, this, [=](int value){
        if (d_progressDialog) {
            d_progressDialog->setProgressValue(value);
        }
    });
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
            QFile *sourceFile = Q_NULLPTR;
            if (headerSize <= 0) {
                QFile _sourceFile(filePath);
                if (!_sourceFile.open(QIODevice::ReadOnly)) {
                    return false;
                }
                QTemporaryFile *sourceTempFile = new QTemporaryFile(filePath);
                if (!sourceTempFile->open()) {
                    delete sourceTempFile;
                    return false;
                }
                //
                if (!Icd::checkData(d_table, headerSize, &_sourceFile, sourceTempFile)) {
                    delete sourceTempFile;
                    return false;
                }
                sourceTempFile->reset();
                sourceFile = sourceTempFile;
            } else {
                sourceFile = new QFile(filePath);
                if (!sourceFile->open(QIODevice::ReadOnly)) {
                    delete sourceFile;
                    return false;
                }
            }
            //
            if (!sourceFile) {
                return false;
            }
            //
            if (!splitFile(sourceFile, targetDir, splitSize, int(d_table->bufferSize()),
                           headerSize, hasTimeFormat)) {
                delete sourceFile;
                return false;
            }

            delete sourceFile;

            return true;
        });
    };
    connect(d_progressDialog, &Icd::JProgressDialog::finished, this, [=,&parseTable](){
        if (d_progressDialog->futureResult()) {
            if (parseTable) {
                parseTable = false;
                d_progressDialog->setWindowTitle(tr("Split file"));
                d_progressDialog->setMessage(tr("Splitting file..."));
                d_progressDialog->setFuture(runAgainFuture());
            } else {
                d_progressDialog->hide();
                d_progressDialog->disconnect(this);
                QString message = tr("Split success!");
                QMessageBox::information(this, tr("Result of split"), message);
                d_progressDialog->deleteLater();
                d_progressDialog = nullptr;
                d_table = Icd::TablePtr();
            }
        } else {
            d_progressDialog->hide();
            d_progressDialog->disconnect(this);
            const QString title = parseTable ? tr("Result of parsing") : tr("Result of splitting");
            const QString message = parseTable ? tr("Parse failure!") : tr("Split failure!");
            QMessageBox::information(this, title, message);
            d_progressDialog->deleteLater();
            d_progressDialog = nullptr;
            d_table = Icd::TablePtr();
        }
    });
    d_progressDialog->setFuture(future);
    d_progressDialog->exec();
    d_progressDialog->disconnect(this);

    return result;
}

bool SplitFileDlg::splitFile(QFile *sourceFile, const QString &targetDir,
                             qint64 splitSize, int tableSize, int headerSize,
                             bool hasTimeFormat)
{
    Q_UNUSED(targetDir);
    if (!sourceFile) {
        return false;
    }

    sourceFile->reset();

    bool result = true;
    int fileIndex = 1;
    const QFileInfo fileInfo(sourceFile->fileName());
    const qint64 dataSize = sourceFile->size() - headerSize;
    const int bufferSize = tableSize + (hasTimeFormat ? 8 : 0);
    const qint64 bufferCount = dataSize / bufferSize;
    uchar *buffer = Q_NULLPTR;

    QByteArray headerContents;
    if (headerSize > 0) {
        headerContents = sourceFile->read(headerSize);
    }

    emit progressRangeChanged(0, int(bufferCount - 1));
    emit progressValueChanged(0);

    const QString baseName = fileInfo.completeBaseName();
    const QString splitPath = fileInfo.absolutePath() + '/' + baseName + "-sp-"
            + QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmsszzz");
    QDir splitDir(splitPath);
    if (!splitDir.exists()) {
        if (!splitDir.mkpath(splitDir.filePath(splitPath))) {
            return false;
        }
    }

    QStringList targetFiles;

    for (qint64 i = 0; i < bufferCount; ++i) {
        if (d_progressDialog && d_progressDialog->isCanceled()) {
            result = false;
            break;
        }
        targetFiles.append(splitPath + QString("/%1 (%2).%3")
                           .arg(baseName)
                           .arg(fileIndex)
                           .arg(fileInfo.suffix()));
        QSaveFile targetFile(targetFiles.last());
        if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            result = false;
            break;
        }

        // header
        if (!headerContents.isEmpty()) {
            targetFile.write(headerContents);
        }

        //
        do {
            buffer = sourceFile->map(headerSize + i * bufferSize, bufferSize);
            if (!buffer) {
                result = false;
                break;
            }
            targetFile.write(reinterpret_cast<const char*>(buffer), bufferSize);
            sourceFile->unmap(buffer);
            ++i;
            //
            emit progressValueChanged(int(i));
        } while (i < bufferCount && targetFile.size() < splitSize);

        if (!result) {
            break;
        }

        if (!targetFile.commit()) {
            result = false;
            break;
        }

        ++fileIndex;

        //
        emit progressValueChanged(int(i));
    }

    if (!result) {
        foreach (auto &file, targetFiles) {
            QFile::remove(file);
        }
    }

    targetFiles.clear();

    return result;
}
