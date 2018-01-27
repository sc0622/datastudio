#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../../..)

QT += qml quick quickcontrols2 serialport concurrent

DEFINES += \
    PRO_CORE \
    ICDMETA_LIB \
    ICDMETA_BUILD \
    ICDCORE_LIB \
    ICDCORE_JSON_LIB \
    ICDCOMM_LIB \
    ICDWORKER_LIB \
    ICDPARSER_LIB

##
DEFINES += \
    COMPILE_LUPDATE

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(icdmeta)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdmeta_global.h \
    icdmeta_inc.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    icdmeta_global.cpp

##
include($$PWD/common/common.pri)
include($$PWD/channel/channel.pri)
include($$PWD/core/core.pri)
include($$PWD/protocol/protocol.pri)

###############################################################
# resource files
###############################################################

RESOURCES += \
    $$PWD/resource/resource.qrc

#-------------------------------------------------
# languages
#-------------------------------------------------

## transactions
TRANSLATIONS += \
    $$PWD/resource/lang/icdmeta_zh_CN.ts

contains(DEFINES, COMPILE_LUPDATE) {
    CONFIG(debug, debug|release) {
        commands = @echo off &
        ## execute
        commands += "$$shell_path($$[QT_INSTALL_PREFIX]/bin/lupdate)" \
            "$$shell_path($$[_PRO_FILE_])" \
            -recursive "$$PWD" \
            -ts "$$TRANSLATIONS" &
        QMAKE_POST_LINK += $$commands
    }
}

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
    dstdir = $$this_dir/include/core/icdmeta
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
