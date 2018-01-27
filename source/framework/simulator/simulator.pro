#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

this_dir = $$shell_path($$absolute_path($$PWD/../../..))

QT     -= core gui
CONFIG -= app_bundle
#CONFIG -= qt
#CONFIG += console
TEMPLATE = app

DESTDIR = $$this_dir/bin

##
win32:{
    lessThan(QT_MAJOR_VERSION, 5):{
        CONFIG(debug, debug|release):TARGET = simulatord
        else:TARGET = simulator
    } else {
        TARGET = $$qtLibraryTarget(simulator)
    }
} else {
    TARGET = $$qtLibraryTarget(simulator)
}

## use JLibrary class
DEFINES += JLIBRARY_MODULE

##
unix {
    LIBS += -ldl
}

# The .h file which was generated for your project. Feel free to hck it.
HEADERS += \

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    main.cpp

OTHER_FILES += \

#######################################################
# Copyright Information
#######################################################

QMAKE_TARGET_COMPANY = "Smartsoft"
QMAKE_TARGET_DESCRIPTION = "ICDSimulator"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2017 Smartsoft Inc."

win32:RC_ICONS = $$PWD/res/app.ico

VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0
VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}

#######################################################
# qmake internal options
#######################################################

CONFIG      += warn_on
#CONFIG      += no_keywords
#CONFIG      += silent
#CONFIG      += QT_NO_DEBUG_OUTPUT

#######################################################
# release/debug mode
#######################################################

win32 {
    # On Windows you can't mix release and debug libraries.
    # The designer is built in release mode. If you like to use it
    # you need a release version.For your own application development you
    # might need a debug version.
    # Enable debug_and_release + build_all if you want to build both.

    CONFIG      += debug_and_release
#    CONFIG      += QT_NO_DEBUG_OUTPUT
    CONFIG += build_all

} else {
#    CONFIG      += release
}

linux-g++ {
#    CONFIG      += separate_debug_info
}

###############################################################
# import libraries
###############################################################

INCLUDEPATH += $$getenv(JFRAME_DIR)/include

###############################################################
# global commands
###############################################################

CONFIG(debug, debug|release):DEFINES += DEVELOP

include($$this_dir/tools/function.pri)
include($$this_dir/setenv.pri)

ROOT = $$section(PWD, '/', -1)
INPUT = $$PWD/res/app.ico

exists($$PWD/*.ignore):DISTFILES += $$PWD/*.ignore

win32|unix: {
    #-------------------------------------------------

    $$compiler($$group($${ROOT}.header), $$group(echo --- console - $$TARGET ---))

    #-------------------------------------------------

    # core
    suffixes = exp ilk lib
    for (suffix, suffixes) {
        item = $${TARGET}*.$${suffix}
        name = $$replace(item, \*|\., '_')
        $$compiler($$group($${ROOT}.$${name}), \
            $$removefile($$this_dir/bin/$${item}?))
    }
}
