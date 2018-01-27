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
SUBDIRS += $$split(systems, ' ')
