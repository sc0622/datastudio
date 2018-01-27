#ifndef JWORD_H
#define JWORD_H

#include "../../../icdparser_global.h"
#include "../joffice.h"

namespace Icd {

// - class JWord -

class JWordPrivate;

class ICDPARSER_EXPORT JWord : public JOffice
{
    Q_OBJECT
public:
    explicit JWord(QObject *parent = 0);
    ~JWord();

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JWord)
    J_DECLARE_PRIVATE(JWord)
};

} // end of namespace Icd

#endif  // JEXCEL_H
