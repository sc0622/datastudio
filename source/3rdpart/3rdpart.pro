#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

##
this_dir = $$PWD/../..

##
systems = $$fromfile($$this_dir/.system, systems)

##
mysubdirs =

## for project editor
contains(systems, editor) {
    mysubdirs +=
}

## for project monitor
contains(systems, monitor) {
    mysubdirs +=
}

## for project simulator
contains(systems, simulator) {
    mysubdirs +=
}

##
SUBDIRS += $$unique(mysubdirs)
