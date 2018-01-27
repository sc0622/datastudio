#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$PWD/../../..

QT += gui concurrent xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += \
    PRO_CORE \
    JWT_LIB \
    ICDWIDGET_LIB \
    ICDWIDGET_BUILD \
    ICDCORE_LIB \
    ICDPARSER_LIB \
    ICDCOMM_LIB \
    ICDWORKER_LIB

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(icdwidget)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdwidget_global.h \
    icdwidget_inc.h \
    coretree_widget.h \
    bindchannel_widget.h \
    private/coretree_widget_p.h \
    private/bindchannel_widget_p.h \
    private/itemworkergroup.h \
    channel_widget.h \
    private/channel_widget_p.h \
    worker_label.h \
    private/worker_label_p.h \
    icd_searchedit.h \
    progressdialog.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    coretree_widget.cpp \
    bindchannel_widget.cpp \
    private/coretree_widget_p.cpp \
    private/bindchannel_widget_p.cpp \
    private/itemworkergroup.cpp \
    channel_widget.cpp \
    private/channel_widget_p.cpp \
    worker_label.cpp \
    private/worker_label_p.cpp \
    icd_searchedit.cpp \
    progressdialog.cpp

##

###############################################################
# resource files
###############################################################

RESOURCES += \
    $$PWD/resource/resource.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

###############################################################
# import files
###############################################################

###############################################################
# global commands
###############################################################

win32|unix: {
    commands += echo --- console - $$TARGET --- &

    excludefile = $$PWD/copy.ignore
    !exists("$$excludefile"):excludefile = "$$this_dir/source/common/copy.ignore"
    !exists("$$excludefile"):error("$$excludefile" is not exists!)
    win32:excludefile = $$replace(excludefile, /, \\)

    ## copy files
    dstdir = $$this_dir/include/core/icdwidget
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) ""$$dstdir"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
