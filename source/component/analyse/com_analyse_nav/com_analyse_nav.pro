#-------------------------------------------------
# This file is generated by the Qt Creator
#-------------------------------------------------

this_dir = $$PWD/../../../..

QT += gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += \
    PRO_COMPONENT \
    JWT_LIB \
    ICDCORE_LIB \
    ICDPARSER_LIB \
    ICDWIDGET_LIB \
    ICDWORKER_LIB

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(com_analyse_nav)

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    $$PWD/comanalysenav.h \
    $$PWD/comanalysenavui.h \
    mainnav_widget.h \
    tablesel_widget.h \
    axissel_widget.h \
    private/convertdatadlg.h \
    private/converttoicdwidget.h \
    private/convertfromicdwidget.h \
    private/paritydatadlg.h \
    private/ConvertToDataAnalyse.h \
    private/SplitFileDlg.h \
    private/ExportDataDlg.h \
    private/ExportOrgDataWidget.h \
    private/IcdListWidget.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    $$PWD/comanalysenav.cpp \
    $$PWD/comanalysenavui.cpp \
    mainnav_widget.cpp \
    tablesel_widget.cpp \
    axissel_widget.cpp \
    private/convertdatadlg.cpp \
    private/converttoicdwidget.cpp \
    private/convertfromicdwidget.cpp \
    private/paritydatadlg.cpp \
    private/ConvertToDataAnalyse.cpp \
    private/SplitFileDlg.cpp \
    private/ExportDataDlg.cpp \
    private/ExportOrgDataWidget.cpp \
    private/IcdListWidget.cpp

#
exists($$PWD/com_analyse_nav.xml) {
    OTHER_FILES += \
        $$PWD/com_analyse_nav.xml
}

#-------------------------------------------------
# import libraries
#-------------------------------------------------

#-------------------------------------------------
# global commands
#-------------------------------------------------

win32|unix: {
    commands += echo --- console - com_analyse_nav --- &

    ## copy files
    dstdir = $$this_dir/component/$$systemdir/com_analyse_nav/
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    srcfile = $$PWD/com_analyse_nav.xml
    win32:srcfile = $$replace(srcfile, /, \\)
    exists("$$srcfile"):commands += $(COPY_FILE) "\"$$srcfile\"" "\"$$dstdir\"" &

    QMAKE_POST_LINK += $$commands
}
