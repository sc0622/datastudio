#ifndef VIEWDELEGATE_H
#define VIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace Edit {

class ViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum DelegateType {
        InvalidDelegate = -1,
        VehicleDelegate,
        SystemDelegate,
        TableDelegate,
        // item delegate
        HeaderDelegate,
        CounterDelegate,
        CheckDelegate,
        FrameCodeDelegate,
        NumericDelegate,
        ArrayDelegate,
        BitMapDelegate,
        BitValueDelegate,
        ComplexDelegate,
        FrameDelegate
    };

    explicit ViewDelegate(QObject *parent = nullptr);
    ~ViewDelegate();

    int delegateType() const;

signals:

public slots:
    void setDelegateType(int delegateType);

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private:
    void setupUi();

    // create cell editor
    //QWidget *createField(QWidget *parent, );

private:
    int delegateType_;
};

}

#endif // VIEWDELEGATE_H
