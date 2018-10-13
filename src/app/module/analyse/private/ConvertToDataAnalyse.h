#ifndef CONVERTTODATAANALYSE_H
#define CONVERTTODATAANALYSE_H

#include <QWidget>
#include "IcdListWidget.h"

class QListWidget;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QRadioButton;
class QFile;
class QSaveFile;
class QTextStream;
class QDoubleSpinBox;
class QCheckBox;
class QSpinBox;
class IcdListWidget;

namespace Icd {

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Item;
typedef std::shared_ptr<Item> ItemPtr;

class Table;
typedef std::shared_ptr<Table> TablePtr;

class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;

class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;

class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;

class JProgressDialog;

} // end of namespace Icd

class ConvertToDataAnalyse : public QWidget
{
    Q_OBJECT
public:
    explicit ConvertToDataAnalyse(QWidget *parent = nullptr);
    ConvertToDataAnalyse(const QString &filePath,
                                  const Icd::TablePtr &table,
                                  const Icd::ObjectPtr &objectItem,
                                  bool hasTimeFormat, int headerSize,
                                  int bitOffset, QWidget *parent = nullptr);

signals:
    void rejected();
    void progressMessageChanged(const QString &message);
    void progressRangeChanged(int minimum, int maximum);
    void progressValueChanged(int value);

public slots:

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool loadData(const QString &domain, int headerSize,
                  bool hasTimeFormat);
    bool convertToATX(QFile *sourceFile, QSaveFile *targetFile,
                      const Icd::TablePtr &table,
                      int headerSize, bool hasTimeFormat);
    bool convertToGBR(QFile *sourceFile, QSaveFile *targetFile,
                      const Icd::TablePtr &table,
                      int headerSize, bool hasTimeFormat);
    bool convertToRAW(QFile *sourceFile, QSaveFile *targetFile,
                      const Icd::TablePtr &table,
                      int headerSize, bool hasTimeFormat);
    bool convertToMRD(QFile *sourceFile, QSaveFile *targetFile,
                      const Icd::TablePtr &table,
                      int headerSize, bool hasTimeFormat);

    bool appendVariable(QTextStream &ts, const ItemInfo &itemInfo);
    bool appendVariable(QTextStream &ts, const Icd::BitItemPtr &bitItem,
                        int bitOffset) const;
    bool appendVariable(QTextStream &ts, const Icd::ComplexItemPtr &complexItem) const;
    bool appendVariable(QTextStream &ts, const Icd::FrameItemPtr &frameItem) const;
    bool appendVariable(QTextStream &ts, const Icd::ItemPtr &item) const;
    bool appendVariable(QTextStream &ts, const Icd::TablePtr &table) const;

    bool parseValue(QTextStream &ts, const QList<ItemInfo> &itemInfos) const;
    bool parseValue(QTextStream &ts, const ItemInfo &itemInfo) const;
    bool parseValue(QTextStream &ts, const Icd::BitItemPtr &bitItem,
                    int bitOffset = -1) const;
    bool parseValue(QTextStream &ts, const Icd::ComplexItemPtr &complexItem) const;
    bool parseValue(QTextStream &ts, const Icd::FrameItemPtr &frameItem) const;
    bool parseValue(QTextStream &ts, const Icd::ItemPtr &item) const;
    bool parseValue(QTextStream &ts, const Icd::TablePtr &table) const;

    QString queryFileTypeSuffix() const;

    int queryChildCount(const QList<ItemInfo> &itemInfos, bool splitBit) const;
    int queryChildCount(const Icd::TablePtr &table) const;
    int queryChildCount(const Icd::FrameItemPtr &frameItem) const;
    int queryChildCount(const Icd::ComplexItemPtr &complexItem) const;
    int queryChildCount(const Icd::ItemPtr &item) const;

private:
    void init();

private:
    IcdListWidget *d_listWidget;
    QRadioButton *d_radioATX;
    QRadioButton *d_radioGBR;
    QRadioButton *d_radioRAW;
    QRadioButton *d_radioMRD;
    QLineEdit *d_editHeader;
    QLineEdit *d_editSource;
    QLineEdit *d_editTarget;
    QDoubleSpinBox *d_spinPkgCntScale;
    QCheckBox *d_checkSplitBit;
    QSpinBox *d_spinPkgStart;
    QSpinBox *d_spinPkgEnd;
    QPushButton *d_buttonConvert;
    QPushButton *d_buttonCancel;
    QString d_filePath;
    Icd::TablePtr d_table;
    Icd::ObjectPtr d_objectItem;
    bool d_hasTimeFormat;
    int d_headerSize;
    int d_bitOffset;
    Icd::JProgressDialog *d_progressDialog;
    int d_progressValue;
    int d_timerUpdate;
};

#endif // CONVERTTODATAANALYSE_H
