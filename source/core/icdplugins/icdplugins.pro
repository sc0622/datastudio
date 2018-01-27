#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../../..)

QT += gui qml
CONFIG += plugin c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

ROOT = $$section(PWD, '/', -1)
INPUT =

DEFINES += \
    PRO_CORE_PLUGINS \
    ICDMETA_LIB \

TARGET = icdcoreplugin

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget($$TARGET)
uri = Icd.Core
DESTDIR = $$this_dir/bin/meta/$$replace(uri, \., /)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdplugins_plugin.h

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    icdplugins_plugin.cpp

##
DISTFILES = qmldir

###############################################################
# resource files
###############################################################

RESOURCES += \

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
    dstdir = $$DESTDIR
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) ""$$dstdir"" &
    win32:srcdir = $$PWD/qmldir
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}

## - generate qmltypes

include($$this_dir/tools/function.pri)
exists($$DESTDIR/$${TARGET}$$dylibsuffix()) {
    ##
    QMLPLUGINDUMP_DIR = $$[QT_INSTALL_PREFIX]/bin
    INPUT = $$PWD/qmldir

    depfiles = \
        $$this_dir/bin/3rdpart/moxa/pcomm$$dylibsuffix() \
        $$this_dir/lib/core/icdcore$${debug_suffix}$$dylibsuffix() \
        $$this_dir/lib/core/icdcomm$${debug_suffix}$$dylibsuffix() \
        $$this_dir/lib/core/icdworker$${debug_suffix}$$dylibsuffix() \
        $$this_dir/lib/core/icdparser$${debug_suffix}$$dylibsuffix() \
        $$this_dir/lib/core/icdmeta$${debug_suffix}$$dylibsuffix() \
        $$this_dir/lib/core/DBAccess$${debug_suffix}$$dylibsuffix()

    win32:CONFIG(release, debug|release) | unix: {
        generateQmlTypes($$group($${ROOT}.qmltypes), $$uri, 1.0, $$this_dir/bin/meta, \
            $$DESTDIR/icdcore.qmltypes, $$depfiles)
    }
}

!exists($$DESTDIR/icdcore.qmltypes) {
    warning('$$DESTDIR/icdcore.qmltypes' is not exists!)
}
