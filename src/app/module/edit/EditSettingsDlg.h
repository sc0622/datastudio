#ifndef EDITSETTINGSDLG_H
#define EDITSETTINGSDLG_H

#include <QDialog>

class QComboBox;
class QSpinBox;

namespace Edit {

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
    QBitArray d_modifyFlags;
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
    SetViewSettings *d_setViewSettings;
};

}

#endif // EDITSETTINGSDLG_H
