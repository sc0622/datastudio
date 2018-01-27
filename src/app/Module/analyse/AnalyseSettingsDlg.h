#ifndef ANALYSESETTINGSDLG_H
#define ANALYSESETTINGSDLG_H

#include <QDialog>

class QComboBox;
class QSpinBox;

namespace Analyse {

// class TreeViewSettings

class TreeViewSettings : public QWidget
{
    Q_OBJECT
public:
    explicit TreeViewSettings(QWidget *parent = nullptr);
    ~TreeViewSettings();

    bool tryAccept();

signals:
    void contentChanged();

public slots:

private:
    bool init();

private:
    QBitArray d_modifyFlags;
    QComboBox *d_comboBoxDeep;
};

// class ChartViewSettings

class ChartViewSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ChartViewSettings(QWidget *parent = nullptr);
    ~ChartViewSettings();

    bool tryAccept();

signals:
    void contentChanged();

public slots:

private:
    bool init();

private:
    QBitArray d_modifyFlags;
    QSpinBox *d_spinBoxColumnCount;
    QSpinBox *d_spinBoxYLabelWidth;
};

// class SettingsDlg

class SettingsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDlg(QWidget *parent = nullptr);
    ~SettingsDlg();

signals:

public slots:

private:
    bool saveConfig();

private:
    TreeViewSettings *d_treeViewSettings;
    ChartViewSettings *d_chartViewSettings;
};

}

#endif // MONITORSETTINGSDLG_H
