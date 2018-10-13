#ifndef JOFFICE_H
#define JOFFICE_H

#include "../../icdparser_global.h"
#include <QIODevice>

#ifdef QT_AXCONTAINER_LIB
class QAxBase;
class QAxObject;
#else
#endif

namespace Icd {

// - class JOffice -

class JOfficePrivate;

class ICDPARSER_EXPORT JOffice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(OfficeType officeType READ officeType)
public:
    enum OfficeType {
        OfficeInvalid = -1,
        OfficeExcel,
        OfficeWord,
    };

    explicit JOffice(OfficeType officeType, QObject *parent = nullptr);
    virtual ~JOffice() override;

    bool isValid() const;

    OfficeType officeType() const;
#ifdef QT_AXCONTAINER_LIB
    QAxObject *application();
    QAxObject *workbooks();
    QAxObject *workbook(int index);
    QAxObject *activeWorkbook();
    QAxObject *sheets();
    QAxObject *sheet(int index);

    QAxObject *prependSheet(const QString &sheetName);
    QAxObject *appendSheet(const QString &sheetName);
    QAxObject *insertSheet(int index, const QString &sheetName);

    bool setSheetCount(int count);
    bool clearSheet();

    bool create();
    bool open(const QString &filePath, QIODevice::OpenMode mode);
    bool saveAs(const QString &filePath);
    bool close();
    bool show(bool enabled = true);

    static bool generateDocumentFile(QAxBase *axBase, const QString &filePath = QString::null);
#else
    //TODO
#endif
Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JOffice)
    J_DECLARE_PRIVATE(JOffice)
};

} // end of namespace Icd

#endif // JOFFICEBASE_H
