#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$PWD/../../..

#QT += gui
QT += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

DEFINES += \
    PRO_CORE \
    ICDWORKER_LIB \
    ICDWORKER_BUILD \
    ICDCORE_LIB \
    ICDCOMM_LIB

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(icdworker)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdworker_global.h \
    icdworker_inc.h \
    icdworker.h \
    icdworker_pool.h \
    jrttimer.h \
    icdworker_recv.h \
    icdworker_send.h \
    private/icdworker_recv_p.h \
    private/icdworker_send_p.h \
    icdworker_util.h \
    icdworker_trans.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    icdworker.cpp \
    icdworker_pool.cpp \
    jrttimer.cpp \
    icdworker_recv.cpp \
    icdworker_send.cpp \
    private/icdworker_recv_p.cpp \
    private/icdworker_send_p.cpp \
    icdworker_util.cpp

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
    dstdir = $$this_dir/include/core/icdworker
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
