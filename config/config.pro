#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

##
this_dir = $$PWD/..

TEMPLATE = aux

##
systems = $$fromfile($$this_dir/.system, systems)

##
for (item, $$list($$systems)) {
    exists($$PWD/$${item}/$${item}.pri):include($$PWD/$${item}/$${item}.pri)
}
