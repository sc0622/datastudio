#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

lessThan(QT_VERSION, 5.6.0):error("Qt version must be greator than 5.6.0!")
!win32:error("Operate system platform must be windows x86!")

SUBDIRS += \
    src \
#    setup

include($$PWD/config/config.pri)

##
DISTFILES += \
    tools/function.pri \
    tools/xcopy.py \
    LICENSE \
    README \
    CHANGE \
    VERSION
