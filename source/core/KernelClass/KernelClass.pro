#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T14:36:51
#
#-------------------------------------------------

this_dir = $$PWD/../../..

QT       -= gui

DEFINES += \
    PRO_CORE \
    KERNELCLASS_LIB \
    KERNELCLASS_BUILD

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(KernelClass)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    datalimitation.h \
    globalstruct.h \
    icdbitdata.h \
    icdcheckdata.h \
    icdcommondata.h \
    icdcomplexdata.h \
    icdcounterdata.h \
    icdcustomizeddata.h \
    icdelement.h \
    icdfactory.h \
    icdframecodedata.h \
    icdmetadata.h \
    planenode.h \
    systemnode.h \
    tablenode.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    datalimitation.cpp \
    icdbitdata.cpp \
    icdcheckdata.cpp \
    icdcommondata.cpp \
    icdcomplexdata.cpp \
    icdcounterdata.cpp \
    icdcustomizeddata.cpp \
    icdelement.cpp \
    icdfactory.cpp \
    icdframecodedata.cpp \
    icdmetadata.cpp \
    planenode.cpp \
    systemnode.cpp \
    tablenode.cpp

###############################################################
# resource files
###############################################################

###############################################################
# import files
###############################################################

#-------------------------------------------------
# global commands
#-------------------------------------------------

win32|unix: {
    commands += echo --- console - KernelClass --- &

    ## copy files
    dstdir = $$this_dir/include/core/KernelClass
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    srcfile = $$PWD/*.h
    win32:srcfile = $$replace(srcfile, /, \\)
    exists("$$srcfile"):commands += $(COPY_FILE) "\"$$srcfile\"" "\"$$dstdir\"" &

    QMAKE_POST_LINK += $$commands
}
