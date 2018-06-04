#ifndef SPLITFILEDLG_H
#define SPLITFILEDLG_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QComboBox;
class QRadioButton;
class QFile;
class QSaveFile;
class QTextStream;
class QCheckBox;

namespace Icd {

template<typename T> class std::shared_ptr;

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Item;
typedef std::shared_ptr<Item> ItemPtr;

class Table;
typedef std::shared_ptr<Table> TablePtr;

class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;

class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;

class ProgressDialog;

} // end of namespace Icd

class SplitFileDlg : public QWidget
{
    Q_OBJECT
public:
    explicit SplitFileDlg(QWidget *parent = Q_NULLPTR);

signals:
    void rejected();
    void progressMessageChanged(const QString &message);
    void progressRangeChanged(int minimum, int maximum);
    void progressValueChanged(int value);

public slots:

private:
    bool splitDataFile(const QString &filePath, const QString targetDir, qint64 splitSize);
    bool splitIcdFile(const QString &filePath, const QString targetDir, qint64 splitSize);
    bool splitATXFile(const QString &filePath, const QString targetDir, qint64 splitSize);
    bool splitGBRFile(const QString &filePath, const QString targetDir, qint64 splitSize);
    bool splitRAWFile(const QString &filePath, const QString targetDir, qint64 splitSize);
    bool splitMRDFile(const QString &filePath, const QString targetDir, qint64 splitSize);

    bool loadTable(const QString &filePath, const QString targetDir, qint64 splitSize,
                   const QString &domain, int headerSize, bool hasTimeFormat);
    bool splitFile(QFile *sourceFile, const QString targetDir, qint64 splitSize,
                   int tableSize, int headerSize, bool hasTimeFormat);

private:
    QComboBox *d_comboFileType;
    QComboBox *d_comboSplitSize;
    QLineEdit *d_editSource;
    QLineEdit *d_editTarget;
    QCheckBox *d_checkAsFTI;
    QPushButton *d_buttonSplit;
    QPushButton *d_buttonCancel;
    Icd::TablePtr d_table;
    Icd::ProgressDialog *d_progressDialog;
};

#endif // SPLITFILEDLG_H
