#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

## systems
#systems = $$getenv(JFRAME_SYSTEMS)
systems = editor;monitor;simulator;analyse;icdsystem
isEmpty(systems) {
    error("Please add systems environment variable 'JFRAME_SYSTEMS'!(split with ';')")
}

##
systems = systems=$$replace(systems, ;, ' ')
write_file($$PWD/.system, systems)

##
SUBDIRS += \
    config \
    source \
    setup

##
DISTFILES += \
    tools/function.pri \
    tools/xcopy.py \
    README \
    CHANGE \
    VERSION
