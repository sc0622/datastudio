#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$PWD/../../..

QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += \
    PRO_CORE \
    JWT_LIB \
    ICDCHART_LIB \
    ICDCHART_BUILD \
    JCHART_LIB \
    ICDCORE_LIB \
    ICDPARSER_LIB \
    ICDWORKER_LIB \
    ICDWIDGET_LIB \
    QWT_LIB

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(icdchart)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdchart_global.h \
    icdchart_inc.h \
    icdchart_view.h \
    private/icdchart_view_p.h \
    icdchart_fileview.h \
    private/icdchart_fileview_p.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    icdchart_view.cpp \
    private/icdchart_view_p.cpp \
    icdchart_fileview.cpp \
    private/icdchart_fileview_p.cpp

##

###############################################################
# resource files
###############################################################

RESOURCES +=

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
    dstdir = $$this_dir/include/core/icdchart
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) ""$$dstdir"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
