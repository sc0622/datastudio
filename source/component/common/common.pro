#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

##
this_dir = $$PWD/../../..

##
systems = $$fromfile($$this_dir/.system, systems)

##
mysubdirs = com_resource

## for project icdsystem
contains(systems, icdsystem) {
    mysubdirs += \
        com_channelmgr \
        com_icdsystem \
        com_utraledit
}

## for project editor
contains(systems, editor) {
    mysubdirs +=
}

## for project monitor
contains(systems, monitor) {
    mysubdirs += \
        com_channelmgr
}

## for project simulator
contains(systems, simulator) {
    mysubdirs += \
        com_channelmgr
}

##
SUBDIRS += $$unique(mysubdirs) \
    com_databasemgr
