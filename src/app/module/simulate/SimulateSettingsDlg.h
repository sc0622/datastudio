#ifndef SIMULATESETTINGSDLG_H
#define SIMULATESETTINGSDLG_H

#include <QDialog>

class QComboBox;
class QSpinBox;

namespace Simulate {

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
    QBitArray modifyFlags_;
    QComboBox *comboBoxDeep_;
};

// class SetViewSettings

class SetViewSettings : public QWidget
{
    Q_OBJECT
public:
    explicit SetViewSettings(QWidget *parent = nullptr);
    ~SetViewSettings();

    bool tryAccept();

signals:
    void contentChanged();

public slots:

private:
    bool init();

private:
    QBitArray modifyFlags_;
    QCheckBox *checkBoxImmUpdate_;
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
    TreeViewSettings *treeViewSettings_;
    SetViewSettings *setViewSettings_;
};

}

#endif // SIMULATESETTINGSDLG_H
