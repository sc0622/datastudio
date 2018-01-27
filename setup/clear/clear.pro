#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T18:09:05
#
#-------------------------------------------------

TEMPLATE = aux
INSTALLER = setup
CONFIG += console

#-------------------------------------------------

this_dir = $$shell_path($$PWD/../..)    #
ROOT = $$section(PWD, '/', -1)
PKG_EXCLUDE =

include($$this_dir/tools/function.pri)

#-------------------------------------------------

$$compiler($$group($${ROOT}), $$group(echo --- $$PWD --- remove $$INSTALLER -----))

#-------------------------------------------------

include($$PWD/clear.pri)
include($$PWD/../pack/packages/packages.pri)

#-------------------------------------------------
