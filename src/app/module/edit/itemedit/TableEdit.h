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
    ~TableEdit() override;

    Icd::TablePtr table() const;

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() override;
    void saveContent() override;

private:
    QComboBox *comboCodeType_;
    JLargeSpinBox *spinCode_;
    QSpinBox *spinSequence_;
};

}

#endif // VEHICLE_EDIT_H
