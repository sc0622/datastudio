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
    mysubdirs *= \
        icdcore \
        icdparser \
        icdcomm \
        icdworker \
        icdwidget \
        DBAccess \
        KernelClass
}

## for project monitor
contains(systems, monitor) {
    mysubdirs *= \
        icdcore \
        icdparser \
        icdcomm \
        icdworker \
        icdchart \
        icdwidget
}

## for project simulator
contains(systems, simulator) {
    mysubdirs *= \
        icdcore \
        icdparser \
        icdcomm \
        icdworker \
        icdwidget
}

##
SUBDIRS *= \
    $$mysubdirs \
    icdmeta \
    icdplugins
