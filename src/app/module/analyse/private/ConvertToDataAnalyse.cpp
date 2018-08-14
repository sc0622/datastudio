#include "precomp.h"
#include "ConvertToDataAnalyse.h"
#include "icdwidget/coretree_widget.h"
#include "icdwidget/progressdialog.h"
#include "icdparser/icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "icdcore/icd_item_bit.h"
#include "icdworker/icdworker_util.h"
#include "../tablesel_widget.h"
#include "IcdListWidget.h"

ConvertToDataAnalyse::ConvertToDataAnalyse(QWidget *parent)
    : QWidget(parent)
    , d_listWidget(nullptr)
    , d_checkSplitBit(nullptr)
    , d_table(nullptr)
    , d_objectItem(nullptr)
    , d_hasTimeFormat(false)
    , d_headerSize(0)
    , d_progressDialog(nullptr)
    , d_progressValue(0)
    , d_timerUpdate(0)
{
    init();
}

ConvertToDataAnalyse::ConvertToDataAnalyse(const QString &filePath,
                                           const Icd::TablePtr &table,
                                           const Icd::ObjectPtr &objectItem,
                                           bool hasTimeFormat, int headerSize,
                                           int bitOffset, QWidget *parent)
    : QWidget(parent)
    , d_listWidget(nullptr)
    , d_checkSplitBit(nullptr)
    , d_filePath(filePath)
    , d_table(table)
    , d_objectItem(objectItem)
    , d_hasTimeFormat(hasTimeFormat)
    , d_headerSize(headerSize)
    , d_bitOffset(bitOffset)
    , d_progressDialog(nullptr)
    , d_progressValue(0)
    , d_timerUpdate(0)
{
    init();
}

void ConvertToDataAnalyse::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_timerUpdate) {
        d_progressDialog->setProgressValue(d_progressValue);
    } else {
        QWidget::timerEvent(event);
    }
}

bool ConvertToDataAnalyse::loadData(const QString &domain, int headerSize,
                                    bool hasTimeFormat)
{
    bool result = true;

    //
    const Icd::ParserPtr parser = JMain::instance()->parser("analyse");
    if (!parser) {
        return false;
    }

    JAutoCursor busyCursor(Qt::BusyCursor);

    d_progressDialog = new Icd::ProgressDialog(this);
    d_progressDialog->setWindowTitle(tr("Load data"));
    d_progressDialog->setCancelVisible(false);
    d_progressDialog->setInvertedAppearance(false);
    connect(this, &ConvertToDataAnalyse::progressMessageChanged,
            this, [=](const QString &message){
        if (d_progressDialog) {
            d_progressDialog->setMessage(message);
        }
    });
    connect(this, &ConvertToDataAnalyse::progressRangeChanged,
            this, [=](int minimum, int maximum){
        if (d_progressDialog) {
            d_progressDialog->setProgressRange(minimum, maximum);
        }
    });
    //
    auto parseFunc = [=]() -> bool {
        if (!parser->parse(domain.section('/', 0, 0).toStdString(),
                           domain.section('/', 1, 1).toStdString(),
                           domain.section('/', 2).toStdString(),
                           d_table, Icd::ObjectItem)) {
            return false;
        }
        return true;
    };
    //
    auto convertFunc = [=]() -> bool {
        const QString sourcePath = d_editSource->text().trimmed();
        const QString targetPath = d_editTarget->text().trimmed();
        QFile *sourceFile = Q_NULLPTR;
        if (d_objectItem) {
            sourceFile = new QFile(d_filePath);
            if (!sourceFile->open(QIODevice::ReadOnly)) {
                delete sourceFile;
                return false;
            }
        } else {
            if (headerSize <= 0) {
                QFile _sourceFile(sourcePath);
                if (!_sourceFile.open(QIODevice::ReadOnly)) {
                    return false;
                }
                QTemporaryFile *sourceTempFile = new QTemporaryFile(sourcePath);
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
                sourceFile = new QFile(sourcePath);
                if (!sourceFile->open(QIODevice::ReadOnly)) {
                    delete sourceFile;
                    return false;
                }
            }
        }
        //
        if (!sourceFile) {
            return false;
        }
        //
        QSaveFile targetFile(targetPath);
        if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            delete sourceFile;
            return false;
        }
        if (d_radioATX->isChecked()) {
            if (!convertToATX(sourceFile, &targetFile, d_table, headerSize, hasTimeFormat)) {
                delete sourceFile;
                return false;
            }
        } else if (d_radioGBR->isChecked()) {
            if (!convertToGBR(sourceFile, &targetFile, d_table, headerSize, hasTimeFormat)) {
                delete sourceFile;
                return false;
            }
        } else if (d_radioRAW->isChecked()) {
            if (!convertToRAW(sourceFile, &targetFile, d_table, headerSize, hasTimeFormat)) {
                delete sourceFile;
                return false;
            }
        } else if (d_radioMRD->isChecked()) {
            if (!convertToMRD(sourceFile, &targetFile, d_table, headerSize, hasTimeFormat)) {
                delete sourceFile;
                return false;
            }
        } else {
            delete sourceFile;
            return false;
        }
        //
        delete sourceFile;
        //
        if (!targetFile.commit()) {
            return false;
        }
        //
        return true;
    };
    //
    bool parseTable = (!d_objectItem);
    if (parseTable) {
        d_progressDialog->setMessage(tr("Parsing data file..."));
        d_progressDialog->setFuture(QtConcurrent::run(parseFunc));
    } else {
        d_progressDialog->setMessage(tr("Converting data file..."));
        d_progressDialog->setFuture(QtConcurrent::run(convertFunc));
    }

    connect(d_progressDialog, &Icd::ProgressDialog::finished, this, [=,&parseTable](){
        if (d_progressDialog->futureResult()) {
            if (parseTable) {
                parseTable = false;
                d_progressDialog->setWindowTitle(tr("Convert data"));
                d_progressDialog->setMessage(tr("Converting data file..."));
                d_progressDialog->setFuture(QtConcurrent::run(convertFunc));
            } else {
                d_progressDialog->hide();
                d_progressDialog->disconnect(this);
                QString message = tr("Convert success!");
                QMessageBox::information(this, tr("Result of converting"), message);
                d_progressDialog->deleteLater();
                d_progressDialog = Q_NULLPTR;
                if (!d_objectItem) {
                    d_table = Icd::TablePtr();
                }
            }
        } else {
            d_progressDialog->hide();
            d_progressDialog->disconnect(this);
            const QString title = parseTable ? tr("Result of parsing") : tr("Result of converting");
            const QString message = parseTable ? tr("Parse failure!") : tr("Convert failure!");
            QMessageBox::information(this, title, message);
            d_progressDialog->deleteLater();
            d_progressDialog = Q_NULLPTR;
            if (!d_objectItem) {
                d_table = Icd::TablePtr();
            }
        }
    });

    if (d_timerUpdate == 0) {
        d_timerUpdate = startTimer(100);
    }

    d_progressDialog->exec();
    d_progressDialog->disconnect(this);
    if (d_timerUpdate > 0) {
        killTimer(d_timerUpdate);
        d_timerUpdate = 0;
    }

    return result;
}

bool ConvertToDataAnalyse::convertToATX(QFile *sourceFile, QSaveFile *targetFile,
                                        const Icd::TablePtr &table,
                                        int headerSize, bool hasTimeFormat)
{
    if (!sourceFile || !targetFile || !table) {
        return false;
    }

    targetFile->reset();

    QTextStream ts(targetFile);

    // first line: comment
    ts << "// " << QString::fromStdString(table->name()) << endl;

    int childCount = 0;

    if (d_objectItem) {
        const QList<ItemInfo> &itemInfos = d_listWidget->items();
        childCount = queryChildCount(itemInfos, d_checkSplitBit->isChecked());
    } else {
        if (d_checkSplitBit->isChecked()) {
            childCount = queryChildCount(table);
        } else {
            childCount = table->childCount();
        }
    }

    if (childCount == 0) {
        Q_ASSERT(false);
        return false;
    }

    ts << childCount + 1 << endl;

    if (hasTimeFormat) {
        ts << "package_datetime ";
    } else {
        ts << "package_count ";
    }

    // variable
    if (d_objectItem) {
        const QList<ItemInfo> &itemInfos = d_listWidget->items();
        foreach (auto &itemInfo, itemInfos) {
            if (!appendVariable(ts, itemInfo)) {
                return false;
            }
        }
    } else {
        if (!appendVariable(ts, table)) {
            return false;
        }
    }

    ts << endl;

    // data
    const double pkgCntScale = d_spinPkgCntScale->value();
    const qint64 dataSize = sourceFile->size() - headerSize;
    const int tableSize = int(table->bufferSize());
    const int bufferSize = tableSize + (hasTimeFormat ? 8 : 0);
    const qint64 bufferCount = dataSize / bufferSize;
    uchar *buffer = Q_NULLPTR;

    emit progressRangeChanged(0, int(bufferCount - 1));
    //emit progressValueChanged(0);
    d_progressValue = 0;

    for (qint64 i = 0; i < bufferCount; ++i) {
        if (d_progressDialog && d_progressDialog->isCanceled()) {
            return false;
        }
        buffer = sourceFile->map(headerSize + i * bufferSize, bufferSize);
        if (!buffer) {
            return false;
        }
        if (hasTimeFormat) {
            // datetime
            ts << *reinterpret_cast<qint64*>(buffer) << ' ';
            // values
            table->setBuffer(reinterpret_cast<char*>(buffer + 8));
        } else {
            // package-count
            ts << (i + 1) * pkgCntScale << ' ';
            //
            table->setBuffer(reinterpret_cast<char*>(buffer));
        }
        table->updateRecv();
        if (d_objectItem) {
            const QList<ItemInfo> &itemInfos = d_listWidget->items();
            if (!parseValue(ts, itemInfos)) {
                sourceFile->unmap(buffer);
                return false;
            }
        } else {
            if (!parseValue(ts, table)) {
                sourceFile->unmap(buffer);
                return false;
            }
        }
        sourceFile->unmap(buffer);
        //
        ts << endl;
        //
        //emit progressValueChanged(i);
        d_progressValue = int(i);
    }

    return true;
}

bool ConvertToDataAnalyse::convertToGBR(QFile *sourceFile, QSaveFile *targetFile,
                                        const Icd::TablePtr &table,
                                        int headerSize, bool hasTimeFormat)
{
    if (!sourceFile || !targetFile || !table) {
        return false;
    }

    targetFile->reset();

    Q_UNUSED(table);
    Q_UNUSED(headerSize);
    Q_UNUSED(hasTimeFormat);
    return false;
}

bool ConvertToDataAnalyse::convertToRAW(QFile *sourceFile, QSaveFile *targetFile,
                                        const Icd::TablePtr &table,
                                        int headerSize, bool hasTimeFormat)
{
    if (!sourceFile || !targetFile || !table) {
        return false;
    }

    targetFile->reset();

    Q_UNUSED(table);
    Q_UNUSED(headerSize);
    Q_UNUSED(hasTimeFormat);
    return false;
}

bool ConvertToDataAnalyse::convertToMRD(QFile *sourceFile, QSaveFile *targetFile,
                                        const Icd::TablePtr &table,
                                        int headerSize, bool hasTimeFormat)
{
    if (!sourceFile || !targetFile || !table) {
        return false;
    }

    targetFile->reset();

    Q_UNUSED(table);
    Q_UNUSED(headerSize);
    Q_UNUSED(hasTimeFormat);
    return false;
}

bool ConvertToDataAnalyse::appendVariable(QTextStream &ts, const ItemInfo &itemInfo)
{
    if (itemInfo.isNull()) {
        return false;
    }

    if (itemInfo.bitOffset >= 0) {
        const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Item>(itemInfo.item);
        if (!bitItem) {
            Q_ASSERT(false);
            return false;
        }
        return appendVariable(ts, bitItem, itemInfo.bitOffset);
    } else {
        return appendVariable(ts, itemInfo.item);
    }
}

bool ConvertToDataAnalyse::appendVariable(QTextStream &ts, const Icd::BitItemPtr &bitItem,
                                          int bitOffset) const
{
    if (!bitItem) {
        return false;
    }

    if (bitOffset >= 0) {
        const QString namePath = IcdListWidget::generateItemNamePath(bitItem, bitOffset);
        if (namePath.isEmpty()) {
            Q_ASSERT(false);
            return false;
        }
        ts << namePath << ' ';
    } else {
        //
        QString namePath = IcdListWidget::generateItemNamePath(bitItem, -1);
        if (namePath.isEmpty()) {
            Q_ASSERT(false);
            return false;
        }
        namePath.append('/');
        //
        QString bitSpec;
        const int bitStart = bitItem->bitStart();
        const int bitEnd = bitStart + bitItem->bitCount();
        for (int i = bitStart; i < bitEnd; ++i) {
            bitSpec = QString::fromStdString(bitItem->specAt(Icd::icd_uint64(i)));
            if (bitSpec.isEmpty()) {
                bitSpec = "<?>";
            } else {
                bitSpec.replace(" ", "*");
            }
            ts << namePath + bitSpec + QString("*[#%1]").arg(i) << ' ';
        }
    }

    return true;
}

bool ConvertToDataAnalyse::appendVariable(QTextStream &ts, const Icd::ComplexItemPtr &complexItem) const
{
    if (!complexItem) {
        return false;
    }

    return appendVariable(ts, complexItem->table());
}

bool ConvertToDataAnalyse::appendVariable(QTextStream &ts, const Icd::FrameItemPtr &frameItem) const
{
    if (!frameItem) {
        return false;
    }

    const Icd::TablePtrMap &tables = frameItem->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        if (!appendVariable(ts, table)) {
            return false;
        }
    }

    return true;
}

bool ConvertToDataAnalyse::appendVariable(QTextStream &ts, const Icd::ItemPtr &item) const
{
    if (!item) {
        return false;
    }

    switch (item->type()) {
    case Icd::ItemComplex:
        return appendVariable(ts, JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item));
    case Icd::ItemFrame:
        return appendVariable(ts, JHandlePtrCast<Icd::FrameItem, Icd::Item>(item));
    case Icd::ItemBitMap:
        if (d_checkSplitBit && !d_checkSplitBit->isChecked()) {
            // not split bit-item
            break;
        }
        return appendVariable(ts, JHandlePtrCast<Icd::BitItem, Icd::Item>(item), -1);
    default:
        break;
    }

    const QString namePath = IcdListWidget::generateItemNamePath(item);
    if (namePath.isEmpty()) {
        Q_ASSERT(false);
        return false;
    }

    ts << namePath << ' ';

    return true;
}

bool ConvertToDataAnalyse::appendVariable(QTextStream &ts, const Icd::TablePtr &table) const
{
    if (!table) {
        return false;
    }

    const Icd::ItemPtrArray &items = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!appendVariable(ts, item)) {
            return false;
        }
    }

    return true;
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts,
                                      const QList<ItemInfo> &itemInfos) const
{
    foreach (auto &itemInfo, itemInfos) {
        if (!parseValue(ts, itemInfo)) {
            return false;
        }
    }

    return true;
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts, const ItemInfo &itemInfo) const
{
    if (itemInfo.isNull()) {
        return false;
    }

    if (itemInfo.bitOffset >= 0) {
        const Icd::BitItemPtr bitItem =
                JHandlePtrCast<Icd::BitItem, Icd::Item>(itemInfo.item);
        if (!bitItem) {
            Q_ASSERT(false);
            return false;
        }
        return parseValue(ts, bitItem, itemInfo.bitOffset);
    } else {
        return parseValue(ts, itemInfo.item);
    }
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts, const Icd::BitItemPtr &bitItem,
                                      int bitOffset) const
{
    if (!bitItem) {
        return false;
    }

    if (bitOffset >= 0) {
        ts << (bitItem->testBit(bitOffset) ? 1 : 0 ) << ' ';
    } else {
        int bitStart = bitItem->bitStart();
        int bitEnd = bitStart + bitItem->bitCount();
        for (int i = bitStart; i < bitEnd; ++i) {
            ts << (bitItem->testBit(i) ? 1 : 0) << ' ';
        }
    }

    return true;
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts,
                                      const Icd::ComplexItemPtr &complexItem) const
{
    if (!complexItem) {
        return false;
    }

    return parseValue(ts, complexItem->table());
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts,
                                      const Icd::FrameItemPtr &frameItem) const
{
    if (!frameItem) {
        return false;
    }

    const Icd::TablePtrMap &tables = frameItem->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        if (!parseValue(ts, table)) {
            return false;
        }
    }

    return true;
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts, const Icd::ItemPtr &item) const
{
    if (!item) {
        return false;
    }

    switch (item->type()) {
    case Icd::ItemBitMap:
        if (d_checkSplitBit && !d_checkSplitBit->isChecked()) {
            // not split bit-item
            break;
        }
        return parseValue(ts, JHandlePtrCast<Icd::BitItem, Icd::Item>(item));
    case Icd::ItemComplex:
        return parseValue(ts, JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item));
    case Icd::ItemFrame:
        return parseValue(ts, JHandlePtrCast<Icd::FrameItem, Icd::Item>(item));
    case Icd::ItemNumeric:
    {
        const Icd::NumericItemPtr numericItem =
                JHandlePtrCast<Icd::NumericItem, Icd::Item>(item);
        if (!numericItem) {
            Q_ASSERT(false);
            return false;
        }
        if (!qFuzzyCompare(numericItem->scale(), 1)) {
            ts << QString("%1").arg(item->data(), 0, 'g', 10) << ' ';
            return true;
        }
    }
    default:
        break;
    }

    ts << item->data() << ' ';

    return true;
}

bool ConvertToDataAnalyse::parseValue(QTextStream &ts, const Icd::TablePtr &table) const
{
    if (!table) {
        return false;
    }

    const Icd::ItemPtrArray &items = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!parseValue(ts, item)) {
            return false;
        }
    }

    return true;
}

QString ConvertToDataAnalyse::queryFileTypeSuffix() const
{
    if (d_radioATX->isChecked()) {
        return ".atx";
    } else if (d_radioGBR->isChecked()) {
        return ".gbr";
    } else if (d_radioRAW->isChecked()) {
        return ".raw";
    } else if (d_radioMRD->isChecked()) {
        return ".mrd";
    } else {
        return QString();
    }
}

int ConvertToDataAnalyse::queryChildCount(const QList<ItemInfo> &itemInfos, bool splitBit) const
{
    int count = 0;

    foreach (auto &itemInfo, itemInfos) {
        if (itemInfo.bitOffset >= 0) {
            ++count;
        } else {
            const Icd::ItemPtr item = itemInfo.item;
            if (item) {
                if (splitBit) {
                    count += queryChildCount(item);
                } else {
                    count += 1 + item->childCount();
                }
            }
        }
    }

    return count;
}

int ConvertToDataAnalyse::queryChildCount(const Icd::TablePtr &table) const
{
    if (!table) {
        return 0;
    }

    int count = 0;

    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (; citer != items.cend(); ++citer) {
        count += queryChildCount(*citer);
    }

    return count;
}

int ConvertToDataAnalyse::queryChildCount(const Icd::FrameItemPtr &frameItem) const
{
    int count = 0;

    const Icd::TablePtrMap &tables = frameItem->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        count += queryChildCount(citer->second);
    }

    return count;
}

int ConvertToDataAnalyse::queryChildCount(const Icd::ComplexItemPtr &complexItem) const
{
    return queryChildCount(complexItem->table());
}

int ConvertToDataAnalyse::queryChildCount(const Icd::ItemPtr &item) const
{
    switch (item->type()) {
    case Icd::ItemFrame:
    {
        return queryChildCount(JHandlePtrCast<Icd::FrameItem, Icd::Item>(item));
    }
    case Icd::ItemComplex:
    {
        return queryChildCount(JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item));
    }
    case Icd::ItemBitMap:
    {
        const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Item>(item);
        if (!bitItem) {
            Q_ASSERT(false);
            return 0;
        }
        return bitItem->bitCount();
    }
    default:
        return 1;
    }
}

void ConvertToDataAnalyse::init()
{
    QVBoxLayout *vertLayoutRight = nullptr;
    if (d_objectItem) {
        QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
        horiLayoutMain->setContentsMargins(3, 3, 3, 3);

        JSplitter *splitter = new JSplitter(this);
        splitter->setHandleWidth(6);
        splitter->setScales(QList<double>() << 1.2 << 1);
        horiLayoutMain->addWidget(splitter);

        d_listWidget = new IcdListWidget(d_table, d_objectItem,
                                         d_filePath, d_bitOffset, this);
        splitter->addWidget(d_listWidget);

        QWidget *widgetRight = new QWidget(this);
        splitter->addWidget(widgetRight);

        vertLayoutRight = new QVBoxLayout(widgetRight);
        vertLayoutRight->setContentsMargins(0, 0, 0, 0);

        d_listWidget->installEventFilter(this);
    } else {
        vertLayoutRight = new QVBoxLayout(this);
        vertLayoutRight->setContentsMargins(3, 3, 3, 3);
    }

    QGroupBox *groupBoxDataType = new QGroupBox(tr("Type of generated file"));
    vertLayoutRight->addWidget(groupBoxDataType);

    QFormLayout *formLayoutDataType = new QFormLayout(groupBoxDataType);
    formLayoutDataType->setLabelAlignment(Qt::AlignRight);

    d_radioATX = new QRadioButton(tr("Text data (ATX, ATXEX)"));
    d_radioGBR = new QRadioButton(tr("Universal binary data (BGR)"));
    d_radioRAW = new QRadioButton(tr("Universal original data (RAW)"));
    d_radioMRD = new QRadioButton(tr("RASS reatime data (MRD)"));
    formLayoutDataType->addRow(" ", d_radioATX);
    formLayoutDataType->addRow(" ", d_radioGBR);
    formLayoutDataType->addRow(" ", d_radioRAW);
    formLayoutDataType->addRow(" ", d_radioMRD);

    QGroupBox *groupBoxFilePath = new QGroupBox(tr("File path"));
    vertLayoutRight->addWidget(groupBoxFilePath);

    QFormLayout *formLayoutFilePath = new QFormLayout(groupBoxFilePath);
    formLayoutFilePath->setLabelAlignment(Qt::AlignRight);

    d_editHeader = new QLineEdit();
    QPushButton *buttonHeaderView = new QPushButton("...");
    buttonHeaderView->setFixedWidth(60);
    QHBoxLayout *horiLayoutHeader = new QHBoxLayout();
    horiLayoutHeader->setContentsMargins(0, 0, 0, 0);
    horiLayoutHeader->addWidget(d_editHeader);
    horiLayoutHeader->addWidget(buttonHeaderView);
    formLayoutFilePath->addRow(tr("Header file:"), horiLayoutHeader);

    d_editSource = new QLineEdit();
    QPushButton *buttonSourceView = new QPushButton("...");
    buttonSourceView->setFixedWidth(60);
    QHBoxLayout *horiLayoutSource = new QHBoxLayout();
    horiLayoutSource->setContentsMargins(0, 0, 0, 0);
    horiLayoutSource->addWidget(d_editSource);
    horiLayoutSource->addWidget(buttonSourceView);
    formLayoutFilePath->addRow(tr("Source file:"), horiLayoutSource);

    d_editTarget = new QLineEdit();
    QPushButton *buttonTargetView = new QPushButton("...");
    buttonTargetView->setFixedWidth(60);
    QHBoxLayout *horiLayoutTarget = new QHBoxLayout();
    horiLayoutTarget->setContentsMargins(0, 0, 0, 0);
    horiLayoutTarget->addWidget(d_editTarget);
    horiLayoutTarget->addWidget(buttonTargetView);
    formLayoutFilePath->addRow(tr("Target file:"), horiLayoutTarget);

    QGroupBox *groupOther = new QGroupBox(tr("Other"), this);
    vertLayoutRight->addWidget(groupOther);

    QFormLayout *formLayoutExt = new QFormLayout(groupOther);
    formLayoutExt->setLabelAlignment(Qt::AlignRight);

    d_spinPkgCntScale = new QDoubleSpinBox(groupOther);
    d_spinPkgCntScale->setDecimals(3);
    d_spinPkgCntScale->setRange(0, 100000000000);
    d_spinPkgCntScale->setValue(0.02);
    formLayoutExt->addRow(tr("Scale of package counter:"), d_spinPkgCntScale);

    d_checkSplitBit = new QCheckBox(" ", groupOther);
    formLayoutExt->addRow(tr("Bit of splitting:"), d_checkSplitBit);

    vertLayoutRight->addStretch();

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutRight->addLayout(horiLayoutBottom);

    d_buttonConvert = new QPushButton(tr("Start converting"), this);
    d_buttonCancel = new QPushButton(tr("Cancel"), this);
    d_buttonConvert->setFixedWidth(120);
    d_buttonCancel->setFixedWidth(120);
    d_buttonConvert->setDefault(true);
    horiLayoutBottom->addWidget(d_buttonConvert);
    horiLayoutBottom->addWidget(d_buttonCancel);

    connect(buttonHeaderView, &QPushButton::clicked, this, [=](){
        QString selectedFilter = tr("Any file (*.*)");
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select header file"), d_editHeader->text(),
                    "Any file (*.*);;"
                    "Data file (*.dat);;"
                    "Text file (*.txt)", &selectedFilter);
        if (filePath.isEmpty() || !QFile::exists(filePath)) {
            return;
        }
        //
        d_editHeader->setText(filePath);
        d_editHeader->setToolTip(filePath);
    });
    connect(buttonSourceView, &QPushButton::clicked, this, [=](){
        QString selectedFilter = tr("Any file (*.*)");
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
                                  .append(fileInfo.completeBaseName())
                                  .append(queryFileTypeSuffix()));
            d_editTarget->setToolTip(d_editTarget->text());
        }
    });
    connect(buttonTargetView, &QPushButton::clicked, this, [=](){
        const QString suffix = queryFileTypeSuffix();
        QString selectedFilter = tr("Data file (*%1)");
        selectedFilter = selectedFilter.arg(suffix);
        const QString filePath = QFileDialog::getSaveFileName(
                    this, tr("Select target file"), d_editTarget->text(),
                    QString("Any file (*.*);;"
                            "Data file (*%1)").arg(suffix), &selectedFilter);
        if (filePath.isEmpty() || !QFile::exists(filePath)) {
            return;
        }
        //
        d_editTarget->setText(filePath);
        d_editTarget->setToolTip(filePath);
    });
    connect(d_buttonConvert, &QPushButton::clicked, this, [=](){
        //
        if (d_objectItem) {
            //
            if (!loadData(QString(), d_headerSize, d_hasTimeFormat)) {
                return;
            }
        } else {
            //
            QFile sourceFile(d_editSource->text().trimmed());
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
        }
    });
    connect(d_buttonCancel, &QPushButton::clicked, this, [=](){
        emit rejected();
    });

    //TODO
    d_radioATX->setChecked(true);
    d_radioGBR->setEnabled(false);
    d_radioRAW->setEnabled(false);
    d_radioMRD->setEnabled(false);
    d_editHeader->setEnabled(false);
    buttonHeaderView->setEnabled(false);

    //
    if (!d_filePath.isEmpty()) {
        d_editSource->setText(d_filePath);
        //
        if (d_editTarget->text().trimmed().isEmpty()) {
            QFileInfo fileInfo(d_filePath);
            d_editTarget->setText(fileInfo.absolutePath().append("/")
                                  .append(fileInfo.completeBaseName())
                                  .append(queryFileTypeSuffix()));
            d_editTarget->setToolTip(d_editTarget->text());
        }
    }
}
