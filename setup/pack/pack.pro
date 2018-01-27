#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T18:09:05
#
#-------------------------------------------------

TEMPLATE = aux
INSTALLER = setup
CONFIG += console ordered

ROOT = $$section(PWD, '/', -1)
INPUT =
PKG_EXCLUDE =

#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../..)    #

DEFINES += PACKAGE          # pack/clear flag

execludes = $$PWD/copy.ignore

#-------------------------------------------------

include($$this_dir/tools/function.pri)

#-------------------------------------------------

$$compiler($$group($${ROOT}.pro.header), \
    $$group(echo --- $$PWD --- generate $$INSTALLER -----))

#-------------------------------------------------

include($$PWD/pack.pri)
include($$PWD/packages/packages.pri)

#-------------------------------------------------

dstdir = $$shell_path("$$PWD/bin/")

!isEmpty(PKG_EXCLUDE):PKG_EXCLUDE = -e $$join(PKG_EXCLUDE, ',')

win32:QTIFW_DIR = $$(QTIFW_DIR)/bin
unix:QTIFW_DIR = /opt/Qt/QtIFW2.0.3/bin
QTIFW_DIR = $$shell_path($$QTIFW_DIR)
commands = $${EOFF} ($$DIR_NOT_EXISTS \"$$dstdir\" $$MKDIR \"$$dstdir\" ) &
commands += echo generating $$INSTALLER file...please wait... &
commands += "$$shell_path($$QTIFW_DIR/binarycreator)" --verbose --offline-only \
    -c "$$PWD/config/config.xml" \
    -p  "$$PWD/packages" "$$PWD/bin/$$INSTALLER" \
    -r "$$PWD/config/config.qrc" \
    $${PKG_EXCLUDE}

INPUT = $$PWD/config/config.xml $$PWD/packages
$$compiler($$group($${ROOT}.setup), $$commands)

#-------------------------------------------------

DISTFILES += \
    copy.ignore \
    config/config.xml \
    config/config.qrc