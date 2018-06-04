#include "precomp.h"
#include "jword.h"
#include "../../../icdparser_global.h"

namespace Icd {

// - class JWordPrivate -

class JWordPrivate
{
public:
    JWordPrivate(JWord *q)
        : J_QPTR(q)
    {

    }

private:
    J_DECLARE_PUBLIC(JWord)
};

// - class JWord -

JWord::JWord(QObject *parent) :
    JOffice(OfficeWord, parent),
    J_DPTR(new JWordPrivate(this))
{

}

JWord::~JWord()
{
    Q_D(JWord);
    delete d;
}

} // end of namespace Icd
