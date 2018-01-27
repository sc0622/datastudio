#ifndef JEXCEL_H
#define JEXCEL_H

#include "joffice_global.h"
#include "jofficebase.h"

// - class JExcel -

class JExcelPrivate;

class JOFFICE_EXPORT JExcel : public JOfficeBase
{
    Q_OBJECT
public:
    explicit JExcel(QObject *parent = 0);
    ~JExcel();

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JExcel)
    J_DECLARE_PRIVATE(JExcel)
};

#endif  // JEXCEL_H
