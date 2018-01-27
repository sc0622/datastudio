#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../../..)

QT += gui xml printsupport

win32 {
    QT += axcontainer
}

DEFINES += \
    PRO_CORE \
    ICDPARSER_LIB \
    ICDPARSER_BUILD \
    ICDCORE_LIB \
    ICDCORE_JSON_LIB \
    ICDWORKER_LIB

## TEST
#DEFINES += PARSE_TABLE_DYNAMIC_ID

include($$this_dir/src/common/build.pri)

TARGET = $$qtLibraryTarget(icdparser)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdparser.h \
    icdparser_global.h \
    icdparser_inc.h


# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    icdparser.cpp

##
include($$PWD/private/private.pri)

###############################################################
# resource files
###############################################################

RESOURCES += \
    resource/resource.qrc

###############################################################
# import files
###############################################################

###############################################################
# global commands
###############################################################

win32|unix: {
    commands += echo --- console - $$TARGET --- &

    excludefile = $$PWD/copy.ignore
    !exists("$$excludefile"):excludefile = "$$this_dir/src/common/copy.ignore"
    !exists("$$excludefile"):error("$$excludefile" is not exists!)
    win32:excludefile = $$replace(excludefile, /, \\)

    ## copy files
    dstdir = $$this_dir/include/core/icdparser
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
