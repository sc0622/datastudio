#include "precomp.h"
#include "joffice.h"
#ifdef QT_AXCONTAINER_LIB
#include <ActiveQt/QAxWidget>
#include <QAxObject>
#endif

#define J_AXBASE_GENERATE_DOCUMENT 0

namespace Icd {

// - class JOfficePrivate -

class JOfficePrivate
{
public:
    JOfficePrivate(JOffice::OfficeType officeType, JOffice *q)
        : J_QPTR(q)
    #ifdef QT_AXCONTAINER_LIB
        , application(nullptr)
        , workbooks(nullptr)
        , workbook(nullptr)
    #endif
        , officeType(officeType)
    {
        //
#ifdef _MSC_VER
        ::OleInitialize(nullptr);
#endif
#ifdef QT_AXCONTAINER_LIB
        application = new QAxObject("Excel.Application");

        switch (officeType) {
        case JOffice::OfficeWord:
            application->setControl("Word.Application");
            break;
        case JOffice::OfficeExcel:
            application = new QAxObject();
            application->setControl("Excel.Application");
            break;
        default:
            break;
        }


        //
        if (application->isNull()) {
            Q_ASSERT(application);
        } else {
            application->setProperty("WindowState", 2);
            application->dynamicCall("SetVisible(bool)", false);
            application->setProperty("DisplayAlerts", false);
            workbooks = application->querySubObject("Workbooks");
        }
#if J_AXBASE_GENERATE_DOCUMENT
        generateDocumentFile(application);
        generateDocumentFile(workbooks);
#endif
#endif  // QT_AXCONTAINER_LIB
    }

    ~JOfficePrivate()
    {
#ifdef QT_AXCONTAINER_LIB
        Q_Q(JOffice);
        q->close();

        delete application;
        application = nullptr;
#endif
#ifdef _MSC_VER
        ::OleUninitialize();
#endif
    }

    bool isValid() const;
#ifdef QT_AXCONTAINER_LIB
    static bool generateDocumentation(QAxBase *axBase, const QString &filePath = QString::null);
#else
#endif
private:
    J_DECLARE_PUBLIC(JOffice)
#ifdef QT_AXCONTAINER_LIB
    QAxObject *application;
    QAxObject *workbooks;
    QAxObject *workbook;
#else
#endif
    JOffice::OfficeType officeType;
};

bool JOfficePrivate::isValid() const
{
#ifdef QT_AXCONTAINER_LIB
    return (application && !application->isNull());
#else
    return false;
#endif
}
#ifdef QT_AXCONTAINER_LIB
bool JOfficePrivate::generateDocumentation(QAxBase *axBase, const QString &filePath)
{
    //
    if (!axBase || axBase->isNull()) {
        return false;
    }

    //
    QString _filePath = filePath;
    if (_filePath.isEmpty()) {
        const QMetaObject *mobj = axBase->metaObject();
        _filePath = QApplication::applicationDirPath().append("/docs/help/active/");
        _filePath.append(mobj->classInfo(mobj->indexOfClassInfo("Interface " + QByteArray::number(1))).value());
        _filePath.append(QString("_%1.html").arg(axBase->control()));
    }

    //
    QFileInfo fileInfo(_filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return false;
        }
    }

    //
    QFile file(_filePath);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }

    //
    QTextStream stream(&file);
    stream << axBase->generateDocumentation() << endl;

    return true;
}
#endif
// - class JOffice -

JOffice::JOffice(OfficeType officeType, QObject *parent) :
    QObject(parent),
    J_DPTR(new JOfficePrivate(officeType, this))
{

}

JOffice::~JOffice()
{
    Q_D(JOffice);
    delete d;
}

bool JOffice::isValid() const
{
    Q_D(const JOffice);
    return d->isValid();
}

JOffice::OfficeType JOffice::officeType() const
{
    Q_D(const JOffice);
    return d->officeType;
}
#ifdef QT_AXCONTAINER_LIB
QAxObject *JOffice::application()
{
    Q_D(JOffice);
    return d->application;
}

QAxObject *JOffice::workbooks()
{
    Q_D(JOffice);
    return d->application->querySubObject("WorkBooks");
}

QAxObject *JOffice::workbook(int index)
{
    QAxObject *workbooks = this->workbooks();
    if (workbooks == nullptr) {
        return nullptr;
    }

    return workbooks->querySubObject("Item(QVariant)", index);
}

QAxObject *JOffice::activeWorkbook()
{
    Q_D(JOffice);
    if (d->workbook) {
        return d->workbook;
    } else {
        return d->application->querySubObject("ActiveWorkBook");
    }
}

QAxObject *JOffice::sheets()
{
    Q_D(JOffice);
    QAxObject *workbook = d->workbook;
    if (workbook == nullptr) {
        workbook = activeWorkbook();
    }

    if (workbook == nullptr) {
        return nullptr;
    }

    return workbook->querySubObject("Sheets");
}

QAxObject *JOffice::sheet(int index)
{
    QAxObject *sheets = this->sheets();
    if (sheets == nullptr) {
        return nullptr;
    }
#if J_AXBASE_GENERATE_DOCUMENT
    generateDocumentFile(sheets);
#endif

    return sheets->querySubObject("Item(QVariant)", index);
}

QAxObject *JOffice::prependSheet(const QString &sheetName)
{
    return insertSheet(1, sheetName);
}

QAxObject *JOffice::appendSheet(const QString &sheetName)
{
    QAxObject *sheets = this->sheets();
    if (sheets == nullptr) {
        return nullptr;
    }

    int sheetCount = sheets->property("Count").toInt();
    QAxObject *lastSheet = sheets->querySubObject("Item(QVariant)", sheetCount);
    if (lastSheet == nullptr) {
        return nullptr;
    }

    QAxObject *newSheet = sheets->querySubObject("Add(QVariant)", lastSheet->asVariant());
    if (newSheet == nullptr) {
        return nullptr;
    }

    lastSheet->dynamicCall("Move(QVariant)", newSheet->asVariant());
    if (!sheetName.isEmpty()) {
        newSheet->setProperty("Name", sheetName);
    }

    return newSheet;
}

QAxObject *JOffice::insertSheet(int index, const QString &sheetName)
{
    QAxObject *sheets = this->sheets();
    if (sheets == nullptr) {
        return nullptr;
    }

    int sheetCount = sheets->property("Count").toInt();
    if (sheetCount <= index) {
        return appendSheet(sheetName);
    }

    QAxObject *beforeSheet = sheets->querySubObject("Item(QVariant)", index);
    if (beforeSheet == nullptr) {
        return nullptr;
    }

    QAxObject *newSheet = sheets->querySubObject("Add(QVariant)", beforeSheet->asVariant());
    if (newSheet == nullptr) {
        return nullptr;
    }

    if (!sheetName.isEmpty()) {
        newSheet->setProperty("Name", sheetName);
    }

    return newSheet;
}

bool JOffice::setSheetCount(int count)
{
    QAxObject *sheets = this->sheets();
    if (sheets == nullptr) {
        return false;
    }

    int sheetCount = sheets->property("Count").toInt();
    if (sheetCount == count) {
        return true;
    }

    clearSheet();

    for (int i = 0; i < count - 1; ++i) {
        appendSheet("");
    }

    return true;
}

bool JOffice::clearSheet()
{
    QAxObject *sheets = this->sheets();
    if (sheets == nullptr) {
        return false;
    }

    int sheetCount = sheets->property("Count").toInt();
    while (sheetCount > 1) {
        QAxObject *sheet = sheets->querySubObject("Item(QVariant)", sheetCount);
        if (sheet) {
            sheet->dynamicCall("Delete()");
        }
        sheetCount = sheets->property("Count").toInt();
    }

    return true;
}

bool JOffice::create()
{
    Q_D(JOffice);
    d->workbooks->dynamicCall("Add(QVariant)", QDir::toNativeSeparators(
                                  QApplication::applicationDirPath()
                                  .append("/../config/office/templates/workbook_template_1.xltx")));
    d->workbook = d->workbooks->querySubObject("Item(QVariant)",
                                               d->workbooks->property("Count"));
    if (!d->workbook) {
        return false;
    }
#if J_AXBASE_GENERATE_DOCUMENT
    generateDocumentFile(d->workbook);

    //
    QAxObject *sheet1 = sheet(1);
    if (sheet1) {
        generateDocumentFile(sheet1);
        QAxObject *range1 = sheet1->querySubObject("Range(QVariant)", "A1:B2");
        if (range1) {
            generateDocumentFile(range1);
            QAxObject *interior = range1->querySubObject("Interior");
            if (interior) {
                generateDocumentFile(interior);
            }
            QAxObject *cellFont = range1->querySubObject("Font");
            if (cellFont) {
                generateDocumentFile(cellFont);
            }
            QAxObject *borders = range1->querySubObject("Borders");
            if (borders) {
                generateDocumentFile(borders);
            }
        }
    }
#endif

    return true;
}

bool JOffice::generateDocumentFile(QAxBase *axBase, const QString &filePath)
{
    return JOfficePrivate::generateDocumentation(axBase, filePath);
}

bool JOffice::open(const QString &filePath, QIODevice::OpenMode mode)
{
    Q_D(JOffice);
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        d->workbook = d->workbooks->querySubObject("Open(QVariant)",
                                                   QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
    } else if ((mode != QIODevice::ReadOnly) && !create()) {
        return false;
    }

    if (d->workbook == nullptr) {
        return false;
    }

    return true;
}

bool JOffice::saveAs(const QString &filePath)
{
    Q_D(JOffice);
    if (d->workbook == nullptr) {
        return false;
    }

    d->workbook->dynamicCall("SaveAs(QVariant)", QDir::toNativeSeparators(filePath));

    return true;
}

bool JOffice::close()
{
    Q_D(JOffice);
    if (d->workbook) {
        d->workbook->dynamicCall("Close()", false);
    }

    d->application->dynamicCall("Quit()");

    return true;
}

bool JOffice::show(bool enabled)
{
    Q_D(JOffice);
    if (d->application->isNull()) {
        return false;
    }

    d->application->setProperty("WindowState", 1);
    d->application->setProperty("Visible", enabled);
    d->application->setProperty("ShowWindowsInTaskbar", enabled);
    return true;
}
#endif // QT_AXCONTAINER_LIB
} // end of namespace Icd
