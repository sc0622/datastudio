#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../../..)

QT -= core gui

DEFINES += \
    PRO_CORE \
    ICDCORE_LIB \
    ICDCORE_BUILD \
    JSON_DLL_BUILD \
    NO_LOG4CPP_LIB

include($$this_dir/src/common/build.pri)

TARGET = $$qtLibraryTarget(icdcore)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    $$PWD/icdcore_inc.h \
    $$PWD/icdcore_global.h \
    $$PWD/icd_item_bit.h \
    $$PWD/icd_item_complex.h \
    $$PWD/icd_item_counter.h \
    $$PWD/icd_item_frame.h \
    $$PWD/icd_item_framecode.h \
    $$PWD/icd_item_limit.h \
    $$PWD/icd_item_numeric.h \
    $$PWD/icd_item.h \
    $$PWD/icd_object.h \
    $$PWD/icd_root.h \
    $$PWD/icd_table.h \
    $$PWD/icd_system.h \
    $$PWD/icd_vehicle.h \
    $$PWD/icd_item_check.h \
    $$PWD/icd_item_header.h \
    $$PWD/private/jprecitimer.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    $$PWD/icdcore_global.cpp \
    $$PWD/icd_item_bit.cpp \
    $$PWD/icd_item_check.cpp \
    $$PWD/icd_item_complex.cpp \
    $$PWD/icd_item_counter.cpp \
    $$PWD/icd_item_frame.cpp \
    $$PWD/icd_item_framecode.cpp \
    $$PWD/icd_item_limit.cpp \
    $$PWD/icd_item_numeric.cpp \
    $$PWD/icd_item.cpp \
    $$PWD/icd_root.cpp \
    $$PWD/icd_table.cpp \
    $$PWD/icd_system.cpp \
    $$PWD/icd_vehicle.cpp \
    $$PWD/icd_object.cpp \
    $$PWD/icd_item_header.cpp \
    $$PWD/private/jprecitimer.cpp

##
include($$PWD/3rdpart/jsoncpp.pri)

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
    !exists("$$excludefile"):excludefile = "$$this_dir/src/common/copy.ignore"
    !exists("$$excludefile"):error("$$excludefile" is not exists!)
    win32:excludefile = $$replace(excludefile, /, \\)

    ## copy files
    dstdir = $$this_dir/include/core/icdcore
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
