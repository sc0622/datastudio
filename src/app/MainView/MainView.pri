HEADERS += \
    $$PWD/MainWindow.h \
    $$PWD/CentralWidget.h \
    $$PWD/MainStatusBar.h \
    $$PWD/MainMenuBar.h \
    $$PWD/MainSettingsDlg.h

SOURCES += \
    $$PWD/MainWindow.cpp \
    $$PWD/CentralWidget.cpp \
    $$PWD/MainStatusBar.cpp \
    $$PWD/MainMenuBar.cpp \
    $$PWD/MainSettingsDlg.cpp

include($$PWD/settings/settings.pri)
