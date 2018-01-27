#ifndef CORETREE_WIDGET_H
#define CORETREE_WIDGET_H

#include <QWidget>
#include "icdwidget_global.h"

class QStandardItem;

namespace Icd {

//
template<typename T> class HandlePtr;

class Parser;
typedef JHandlePtr<Parser> ParserPtr;

//
class Table;
typedef JHandlePtr<Table> TablePtr;

// class CoreTreeWidget

class CoreTreeWidgetPrivate;

/**
 * @brief The CoreTreeWidget class
 */
class ICDWIDGET_EXPORT CoreTreeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
public:
    /**
     * @brief The BindTableType enum
     */
    enum BindTableType {
        BindOnlySend = 0x01,
        BindOnlyRecv = 0x02,
        BindAllTable = BindOnlySend | BindOnlyRecv
    };
    Q_DECLARE_FLAGS(BindTableTypes, BindTableType)

    /**
     * @brief The TreeMode enum
     */
    enum TreeMode {
        TreeModeEdit = 0x0001,          /**< . */
        TreeModeMonitor = 0x0002,       /**< . */
        TreeModeSimulator = 0x0004,     /**< . */
        TreeModeAnalyse = 0x0008,       /**< . */
        TreeModeTableSel = 0x0010       /**< . */
    };
    Q_DECLARE_FLAGS(TreeModes, TreeMode)

    /**
     * @brief The ShowAttribute enum
     */
    enum ShowAttribute {
        ShowOffset = 0x0001,    /**< . */
        ShowType = 0x0002,      /**< . */
        ShowData = 0x0004,      /**< . */
        ShowValue = 0x0008,     /**< . */
        ShowSpec = 0x0010      /**< . */
    };
    Q_DECLARE_FLAGS(ShowAttributes, ShowAttribute)

    /**
     * @brief CoreTreeWidget
     * @param parent
     */
    explicit CoreTreeWidget(QWidget *parent = 0);

    virtual ~CoreTreeWidget();

    /**
     * @brief bindTableType
     * @return
     */
    BindTableTypes bindTableType() const;

    /**
     * @brief setBindTableType
     * @param value
     */
    void setBindTableType(BindTableTypes value);

    /**
     * @brief showAttributes
     * @return
     */
    ShowAttributes showAttributes() const;

    /**
     * @brief setShowAttributes
     * @param attrs
     */
    void setShowAttributes(ShowAttributes attrs);

    /**
     * @brief setShowAttribute
     * @param attr
     * @param on
     */
    void setShowAttribute(ShowAttribute attr, bool on = true);

    /**
     * @brief testShowAttribute
     * @param attr
     * @return
     */
    bool testShowAttribute(ShowAttribute attr) const;

    /**
     * @brief dataFormat
     * @return
     */
    int dataFormat() const;

    /**
     * @brief treeModes
     * @return
     */
    TreeModes treeModes() const;

    /**
     * @brief setTreeMode
     * @param modes
     */
    void setTreeMode(TreeModes modes);

    /**
     * @brief setTreeMode
     * @param mode
     * @param on
     */
    void setTreeMode(TreeMode mode, bool on = true);

    /**
     * @brief testTreeMode
     * @param mode
     * @return
     */
    bool testTreeMode(TreeMode mode) const;

    /**
     * @brief parser
     * @return
     */
    Icd::ParserPtr parser() const;

    /**
     * @brief setParser
     * @param parser
     */
    void setParser(const Icd::ParserPtr &parser);

    /**
     * @brief loadingDeep
     * @return
     */
    int loadingDeep() const;

    /**
     * @brief setLoadingDeep
     * @param deep
     */
    void setLoadingDeep(int deep);

    /**
     * @brief intervalUpdate
     * @return
     */
    int intervalUpdate() const;

    /**
     * @brief setIntervalUpdate
     * @param interval
     */
    void setIntervalUpdate(int interval);

    /**
     * @brief clearContents
     */
    void clearContents();

    /**
     * @brief loadData
     * @return
     */
    bool loadData();

    /**
     * @brief loadData
     * @param table
     * @param domain
     * @return
     */
    bool loadData(const Icd::TablePtr &table, const QString &domain = QString());

    /**
     * @brief loadData
     * @param table
     * @param filePath
     * @param hasTimeFormat
     * @param headerSize
     * @param domain
     * @return
     */
    bool loadData(const Icd::TablePtr &table,
                  const QString &filePath,
                  bool hasTimeFormat, int headerSize,
                  const QString &domain = QString());

    /**
     * @brief currentItem
     * @return
     */
    QStandardItem *currentItem() const;

    /**
     * @brief isRunning
     * @return
     */
    bool isRunning() const;

    /**
     * @brief bindingChannels
     */
    void bindingChannels(const QString &filePath);

    /**
     * @brief unbindingChannels
     */
    void unbindingChannels();

    /**
     * @brief exportBindingStatus
     * @param filePath
     */
    void exportBindingStatus(const QString &filePath);

    /**
     * @brief runAllChannels
     */
    void runAllChannels();

    /**
     * @brief stopAllChannels
     */
    void stopAllChannels();

signals:
    void itemPressed(QStandardItem *item);
    void itemClicked(QStandardItem *item);
    void itemDoubleClicked(QStandardItem *item);
    void currentItemChanged(QStandardItem *current, QStandardItem *previous);
    void itemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void channelBound(QStandardItem *item, const QString &channelId);
    void channelUnbound(QStandardItem *item, const QString &channelId);
    void channelChanged(QStandardItem *item, const QString &channelId);
    void unbindItem(QStandardItem *item, QStandardItem *itemTable);
    void settingAxisTriggled();
    void runningChanged(bool value);
    void exportAnalyseData(QStandardItem *item, const QString &filePath,
                           bool hasTimeFormat, int headerSize);

public slots:
    /**
     * @brief setRunning
     * @param value
     */
    void setRunning(bool value);

    /**
     * @brief setDataFormat
     * @param format
     */
    void setDataFormat(int format);

private:
    Q_DISABLE_COPY(CoreTreeWidget)
    J_DECLARE_PRIVATE(CoreTreeWidget)
};

} // end of namespace Icd

#endif // CORETREE_WIDGET_H
