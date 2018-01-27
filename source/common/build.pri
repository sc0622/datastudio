#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../..)
QT += core
TEMPLATE = lib

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11

## fixed on Windowx XP
#win32:QMAKE_LFLAGS_WINDOW = /SUBSYSTEM:WINDOW,5.02

jframe_dir = $$getenv(jframe_dir)
jframe_dir = $$replace(jframe_dir, \\\\, /)

##################################################
# creating a precompiled header file (only supported
# on some platforms (Windows - all MSVC project types,
# Mac OS X - Xcode, Makefile, Unix - gcc 3.3 and up)
##################################################

INCLUDEPATH += $$PWD

win32 {
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD

    contains(DEFINES, J_NO_PRECOMPILED) {
        ##
    } else {
        CONFIG += precompile_header
        PRECOMPILED_HEADER = $$PWD/precomp.h
    }
}

##################################################
# qmake internal options
##################################################

CONFIG      += warn_on
#CONFIG      += no_kewords
#CONFIG      += slient

##################################################
# release/debug mode
##################################################

win32 {
    # On Windows you can't mix release and debug libraries.
    # The designer is built in release mode. If you like to use it
    # you need a release version.For your own application development you
    # might need a debug version.
    # Enable debug_and_release + build_all if you want to build both.

    CONFIG      += debug_and_release
#    CONFIG      += QT_NO_DEBUG_OUTPUT

} else {
#    CONFIG      += release
}

linux-g++ {
#    CONFIG      += separate_debug_info
}

##################################################
# other files
##################################################

contains(DEFINES, PRO_CORE) {
    DESTDIR = $$this_dir/lib/core

    ##
    win32:CONFIG += build_all

    win32 {
        !contains(DEFINES, NO_LOG4CPP_LIB) {
            DEFINES += \
                LOG4CPP_LIB
        }
    }
}

contains(DEFINES, PRO_CORE_PLUGINS) {
    DESTDIR = $$this_dir/bin/qml/$$replace(uri, \., /)

    ##
    win32:CONFIG += build_all
}

contains(DEFINES, PRO_COMPONENT) {
    ##
    win32:CONFIG += build_all

    ##
    contains(DEFINES, JFRAME_COMPONENT) {
        ##
        DESTDIR = $$this_dir/component/jframe/$$TARGET
    } else {
        ##
        systemdir = $$section(_PRO_FILE_PWD_, /, -2, -2)
        ##
        DESTDIR = $$this_dir/component/$$systemdir/$$TARGET
    }

    !contains(DEFINES, NO_JFRAME_LIBS) {
        DEFINES += \
            JFRAME_FACADE_LIB \
            JFRAME_FACTORY_LIB \
            JFRAME_KERNEL_LIB
    }
}

##################################################
# import libraries
##################################################

INCLUDEPATH += \
    $$this_dir/include \
    $$this_dir/include/3rdpart \
    $$this_dir/include/core \
    $$jframe_dir/include \
    $$jframe_dir/include/3rdpart \
    $$jframe_dir/include/core

DEPENDPATH += \
    $$this_dir/include \
    $$this_dir/include/3rdpart \
    $$this_dir/include/core \
    $$jframe_dir/include \
    $$jframe_dir/include/3rdpart \
    $$jframe_dir/include/core

contains(DEFINES, JFRAME_FACADE_LIB) {
    ## import jframe_facade library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/core -ljframe_facade
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/core -ljframe_facaded
    else:unix:LIBS += -L$$jframe_dir/lib/core -ljframe_facade
}

contains(DEFINES, JFRAME_FACTORY_LIB) {
    ## import jframe_factory library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/core -ljframe_factory
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/core -ljframe_factoryd
    else:unix:LIBS += -L$$jframe_dir/lib/core -ljframe_factory
    INCLUDEPATH += $$jframe_dir/include/core/factory
    DEPENDPATH += $$jframe_dir/include/core/factory
}

contains(DEFINES, JFRAME_KERNEL_LIB) {
    ## import jframe_layout library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/core -ljframe_kernel
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/core -ljframe_kerneld
    else:unix:LIBS += -L$$jframe_dir/lib/core -ljframe_kernel
    INCLUDEPATH += $$jframe_dir/include/core/kernel
    DEPENDPATH += $$jframe_dir/include/core/kernel
}

contains(DEFINES, TINYXML_LIB) {
    ## import tinyxml library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ltinyxml
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ltinyxmld
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -ltinyxml
    INCLUDEPATH += $$jframe_dir/include/3rdpart/tinyxml
    DEPENDPATH += $$jframe_dir/include/3rdpart/tinyxml
}

contains(DEFINES, QTWINMIGRATE_LIB) {
    ## import qtwinmigrate library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -lqtwinmigrate
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -lqtwinmigrated
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -lqtwinmigrate
    INCLUDEPATH += $$jframe_dir/include/3rdpart/qtwinmigrate
    DEPENDPATH += $$jframe_dir/include/3rdpart/qtwinmigrate
    win32:DEFINES += _AFXDLL
}

contains(DEFINES, QTRIBBON_LIB) {
    ## import qtribbon library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -lqtribbon
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -lqtribbond
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -lqtribbon
    INCLUDEPATH += $$jframe_dir/include/3rdpart/qtribbon
    DEPENDPATH += $$jframe_dir/include/3rdpart/qtribbon
}

contains(DEFINES, LOG4CPP_LIB) {
    ## import log4cpp library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -llog4cpp
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -llog4cppd
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -llog4cpp
    INCLUDEPATH += $$jframe_dir/include/3rdpart/log4cpp
    DEPENDPATH += $$jframe_dir/include/3rdpart/log4cpp
}

contains(DEFINES, JWT_LIB) {
    ## import jwt library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljwt
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljwtd
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -ljwt
    INCLUDEPATH += $$jframe_dir/include/3rdpart/jwt
    DEPENDPATH += $$jframe_dir/include/3rdpart/jwt
}

contains(DEFINES, JCHART_LIB) {
    ## import jchart library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljchart
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljchartd
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -ljchart
    INCLUDEPATH += $$jframe_dir/include/3rdpart/jchart
    DEPENDPATH += $$jframe_dir/include/3rdpart/jchart
}

contains(DEFINES, JUTRALEDIT_LIB) {
    ## import jutraledit library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljutraledit
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljutraleditd
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -ljutraledit
    INCLUDEPATH += $$jframe_dir/include/3rdpart/jutraledit
    DEPENDPATH += $$jframe_dir/include/3rdpart/jutraledit
}

contains(DEFINES, JENCRYPT_LIB) {
    ## import jencrypt library
    win32:CONFIG(release, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljencrypt
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$jframe_dir/lib/3rdpart -ljencryptd
    else:unix:LIBS += -L$$jframe_dir/lib/3rdpart -ljencrypt
    INCLUDEPATH += $$jframe_dir/include/3rdpart/jencrypt
    DEPENDPATH += $$jframe_dir/include/3rdpart/jencrypt
}

contains(DEFINES, BCG_LIB) {
    ## import BCGCBPro library
    error(not supported!)
}

###########################################################################################

contains(DEFINES, ICDCORE_LIB):!contains(DEFINES, ICDCORE_BUILD) {
    ## import icdcore library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdcore
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdcored
    else:unix:LIBS += -L$$this_dir/lib/core -licdcore
    INCLUDEPATH += \
        $$this_dir/include/core/icdcore \
        $$this_dir/include/core/icdcore/3rdpart
    DEPENDPATH += \
        $$this_dir/include/core/icdcore \
        $$this_dir/include/core/icdcore/3rdpart
}

contains(DEFINES, ICDPARSER_LIB):!contains(DEFINES, ICDPARSER_BUILD) {
    ## import icdparser library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdparser
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdparserd
    else:unix:LIBS += -L$$this_dir/lib/core -licdparser
    INCLUDEPATH += $$this_dir/include/core/icdparser
    DEPENDPATH += $$this_dir/include/core/icdparser
}

contains(DEFINES, ICDCOMM_LIB):!contains(DEFINES, ICDCOMM_BUILD) {
    ## import icdcomm library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdcomm
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdcommd
    else:unix:LIBS += -L$$this_dir/lib/core -licdcomm
    INCLUDEPATH += $$this_dir/include/core/icdcomm
    DEPENDPATH += $$this_dir/include/core/icdcomm
}

contains(DEFINES, ICDWORKER_LIB):!contains(DEFINES, ICDWORKER_BUILD) {
    ## import icdworker library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdworker
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdworkerd
    else:unix:LIBS += -L$$this_dir/lib/core -licdworker
    INCLUDEPATH += $$this_dir/include/core/icdworker
    DEPENDPATH += $$this_dir/include/core/icdworker
}

contains(DEFINES, ICDCHART_LIB):!contains(DEFINES, ICDCHART_BUILD) {
    ## import icdchart library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdchart
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdchartd
    else:unix:LIBS += -L$$this_dir/lib/core -licdchart
    INCLUDEPATH += $$this_dir/include/core/icdchart
    DEPENDPATH += $$this_dir/include/core/icdchart
}

contains(DEFINES, ICDWIDGET_LIB):!contains(DEFINES, ICDWIDGET_BUILD) {
    ## import icdwidget library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdwidget
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdwidgetd
    else:unix:LIBS += -L$$this_dir/lib/core -licdwidget
    INCLUDEPATH += $$this_dir/include/core/icdwidget
    DEPENDPATH += $$this_dir/include/core/icdwidget
}

contains(DEFINES, DBACCESS_LIB):!contains(DEFINES, DBACCESS_BUILD) {
    ## import DBAccess library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -lDBAccess
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -lDBAccessd
    else:unix:LIBS += -L$$this_dir/lib/core -lDBAccess
    INCLUDEPATH += $$this_dir/include/core/DBAccess
    DEPENDPATH += $$this_dir/include/core/DBAccess
}

contains(DEFINES, KERNELCLASS_LIB):!contains(DEFINES, KERNELCLASS_BUILD) {
    ## import KernelClass library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -lKernelClass
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -lKernelClassd
    else:unix:LIBS += -L$$this_dir/lib/core -lKernelClass
    INCLUDEPATH += $$this_dir/include/core/KernelClass
    DEPENDPATH += $$this_dir/include/core/KernelClass
}

## ----------------------------------------------

contains(DEFINES, ICDMETA_LIB):!contains(DEFINES, ICDMETA_BUILD) {
    ## import icdmeta library
    win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib/core -licdmeta
    else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib/core -licdmetad
    else:unix:LIBS += -L$$this_dir/lib/core -licdmeta
    INCLUDEPATH += $$this_dir/include/core/icdmeta
    DEPENDPATH += $$this_dir/include/core/icdmeta
}
