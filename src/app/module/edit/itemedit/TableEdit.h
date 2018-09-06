#ifndef TABLE_EDIT_H
#define TABLE_EDIT_H

#include "ObjectEdit.h"

namespace Icd {
class Table;
typedef std::shared_ptr<Table> TablePtr;
}

class QComboBox;
class JLargeSpinBox;
class QSpinBox;

namespace Edit {

// class TableEdit

class TableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit TableEdit(const Icd::TablePtr &table, QWidget *parent = nullptr);
    ~TableEdit();

    Icd::TablePtr table() const;

    bool init() override;

signals:

public slots:

private:
    QComboBox *comboCodeType_;
    JLargeSpinBox *spinCode_;
    QSpinBox *spinBoxSequence_;
};

}

#endif // VEHICLE_EDIT_H
