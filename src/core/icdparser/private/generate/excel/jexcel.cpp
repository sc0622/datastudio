#include "precomp.h"
#include "jexcel.h"

// - class JExcelPrivate -

class JExcelPrivate
{
    J_DECLARE_PUBLIC(JExcel)
public:
    JExcelPrivate(JExcel *parent) :
        q_ptr(parent)
    {

    }

private:
};

// - class JExcel -

JExcel::JExcel(QObject *parent) :
    JOfficeBase(OfficeExcel, parent),
    d_ptr(new JExcelPrivate(this))
{

}

JExcel::~JExcel()
{
    Q_D(JExcel);
    delete d;
}
