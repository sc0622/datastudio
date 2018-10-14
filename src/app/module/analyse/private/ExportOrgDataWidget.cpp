#include "precomp.h"
#include "ExportOrgDataWidget.h"
#include "icdcore/icd_table.h"

ExportOrgDataWidget::ExportOrgDataWidget(const QString &filePath,
                                         const Icd::TablePtr &table,
                                         const Icd::ObjectPtr &objectItem,
                                         bool hasTimeFormat, int headerSize,
                                         int bitOffset, QWidget *parent)
    : QWidget(parent)
    , d_filePath(filePath)
    , d_table(table)
    , d_objectItem(objectItem)
    , d_hasTimeFormat(hasTimeFormat)
    , d_headerSize(headerSize)
    , d_bitOffset(bitOffset)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    layoutMain->addLayout(formLayout);

    QHBoxLayout *layoutPath = new QHBoxLayout();
    formLayout->addRow(tr("Path of export:"), layoutPath);

    QLineEdit *editPath = new QLineEdit(this);
    QPushButton *buttonView = new QPushButton("...", this);

    buttonView->setFixedWidth(60);
    layoutPath->addWidget(editPath);
    layoutPath->addWidget(buttonView);
    QCheckBox *checkBox = nullptr;

    if (hasTimeFormat) {
        checkBox = new QCheckBox(this);
        formLayout->addRow(tr("Export package time"), checkBox);
    }

    QHBoxLayout *layoutBottom = new QHBoxLayout();
    layoutMain->addLayout(layoutBottom);

    layoutBottom->addStretch();

    QPushButton *buttonOk = new QPushButton(tr("Ok"), this);
    buttonOk->setMinimumWidth(120);
    buttonOk->setEnabled(false);
    layoutBottom->addWidget(buttonOk);

    layoutBottom->addSpacing(20);

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonCancel->setMinimumWidth(120);
    layoutBottom->addWidget(buttonCancel);

    layoutBottom->addStretch();

    connect(editPath, &QLineEdit::textChanged, [=](const QString &text){
        if (text.isEmpty()) {
            buttonOk->setEnabled(false);
        } else {
            buttonOk->setEnabled(true);
        }
    });
    connect(buttonView, &QPushButton::clicked, [=](){
        QString selectedFilter = tr("Data file (*.dat)");
        const QString targetFilePath =
                QFileDialog::getSaveFileName(this, tr("Save as"),
                                             editPath->text(),
                                             "Any file (*.*);;"
                                             "ICD file (*.icd);;"
                                             "Data file (*.dat);;"
                                             "Text file (*.txt)", &selectedFilter);
        if (targetFilePath.isEmpty()) {
            return;
        }
        editPath->setText(targetFilePath);
        editPath->setToolTip(targetFilePath);
    });
    connect(buttonOk, &QPushButton::clicked, [=](){
        const QString target = editPath->text().trimmed();
        if (target.isEmpty()) {
            editPath->selectAll();
            editPath->setFocus();
            return;
        }
        // export
        bool exportTime = false;
        if (checkBox) {
            exportTime = checkBox->isChecked();
        }
        //
        exportData(target, exportTime);
    });
    connect(buttonCancel, &QPushButton::clicked, [=](){
        emit rejected();
    });
}

void ExportOrgDataWidget::exportData(const QString &target, bool exportTime)
{
    if (target.isEmpty()) {
        return;
    }

    //
    int bufferOffset = 0, bufferSize = 0;
    switch (d_objectItem->objectType()) {
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(d_objectItem);
        if (!table) {
            return;
        }
        bufferOffset = int(table->bufferOffset());
        bufferSize = qCeil(table->bufferSize());
        break;
    }
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(d_objectItem);
        if (!item) {
            return;
        }
        bufferOffset = int(item->bufferOffset());
        bufferSize = qCeil(item->bufferSize());
        break;
    }
    default:
        return;
    }

    Icd::JProgressDialog *progressDialog = new Icd::JProgressDialog(this);
    progressDialog->setWindowTitle(tr("Export data"));
    progressDialog->setProgressValue(100);
    progressDialog->setCancelVisible(false);
    progressDialog->setMessage(tr("Exporting data..."));
    QFuture<bool> future = QtConcurrent::run([=]() -> bool {
        QFile sourceFile(d_filePath);
        QFile targetFile(target);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            return false;
        }
        if (!targetFile.open(QIODevice::WriteOnly)) {
            return false;
        }
        targetFile.resize(0);
        qint64 bufferSize = sourceFile.size() - d_headerSize;
        int tableSize = qCeil(d_table->bufferSize());
        if (d_hasTimeFormat) {
            tableSize += 8;
            const int count = int(bufferSize / tableSize);
            for (int i = 0; i < count; ++i) {
                if (exportTime) {
                    sourceFile.seek(d_headerSize + tableSize * i);
                    targetFile.write(sourceFile.read(8));
                }
                sourceFile.seek(d_headerSize + tableSize * i + 8 + bufferOffset);
                targetFile.write(sourceFile.read(bufferSize));
            }
        } else {
            const int count = int(bufferSize / tableSize);
            for (int i = 0; i < count; ++i) {
                sourceFile.seek(d_headerSize + tableSize * i + bufferOffset);
                targetFile.write(sourceFile.read(bufferSize));
            }
        }
        return true;
    });
    connect(progressDialog, &Icd::JProgressDialog::finished, this, [=](){
        progressDialog->hide();
        progressDialog->disconnect(this);
        const QString message = progressDialog->futureResult()
                ? tr("Export success!") : tr("Export failure!");
        QMessageBox::information(this, tr("Result of export"), message);
        progressDialog->deleteLater();
    });
    progressDialog->setFuture(future);
    progressDialog->exec();
}
