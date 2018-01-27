#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$PWD/../../..

QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

DEFINES += \
    PRO_CORE \
    ICDCOMM_LIB \
    ICDCOMM_BUILD \
    JWT_LIB \
    SERIAL_USE_PCOMM

!contains(DEFINES, SERIAL_USE_PCOMM): QT += serialport

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(icdcomm)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    $$PWD/icdcomm_global.h \
    $$PWD/icdcomm_inc.h \
    $$PWD/icdcomm_channel.h \
    $$PWD/icdcomm_filechannel.h \
    $$PWD/icdcomm_serialchannel.h \
    $$PWD/icdcomm_udpchannel.h \
    $$PWD/icdcomm_ringbuffer.h \
    $$PWD/private/icdcomm_udpchannel_p.h \
    $$PWD/private/jserialport.h \
    private/jcircularbuffer.h


# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    $$PWD/icdcomm_channel.cpp \
    $$PWD/icdcomm_filechannel.cpp \
    $$PWD/icdcomm_udpchannel.cpp \
    $$PWD/icdcomm_serialchannel.cpp \
    $$PWD/icdcomm_ringbuffer.cpp \
    $$PWD/private/icdcomm_udpchannel_p.cpp \
    $$PWD/private/jserialport.cpp \
    private/jcircularbuffer.cpp

##

###############################################################
# resource files
###############################################################

RESOURCES +=

###############################################################
# import files
###############################################################

contains(DEFINES, SERIAL_USE_PCOMM) {
    ## import pcomm library
    win32:CONFIG(release, debug|release):LIBS += -L$$PWD/private/moxa -lpcomm
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$PWD/private/moxa -lpcomm
    INCLUDEPATH += $$PWD/private/moxa
    DEPENDPATH += $$PWD/private/moxa
}

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
    dstdir = $$this_dir/include/core/icdcomm
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
