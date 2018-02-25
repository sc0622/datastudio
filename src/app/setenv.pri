#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../..)
QTDIR = $$getenv(QTDIR)
ROOT = $$section(PWD, '/', -1)
INPUT =

!exists($$QTDIR):error("QTDIR is invalid")

exists($$this_dir/*.ignore):DISTFILES += $$this_dir/*.ignore

SETENV_FLAGS += \
    JFRAME_DEV \
    MODULE_3RDPART \
    MODULE_CORE

DEFINES += \
    DEVELOP

#-------------------------------------------------

$$compiler($$group($${ROOT}.$${TARGET}.header), $$group(echo --- $$PWD setenv))

#-------------------------------------------------

## -- libs - JFRAME_DEV
contains(SETENV_FLAGS, JFRAME_DEV) {

    COMPILERS_HEADER = $${ROOT}.jframe.dev
    jframe_dir = $$getenv(JFRAME_DIR)
    !exists($$jframe_dir) {
        jframe_dir = $$absolute_path($$this_dir/../jframework)
    }

    exists($$jframe_dir) {

        DEFINES_TEMP = $$DEFINES
        DEFINES *= DEVELOP

        ##
        $$compiler($$group($$COMPILERS_HEADER), $$group(echo --- $$PWD -))

        ## - 3rdpart - include/lib
        items = jchart jencrypt jutraledit jwt log4cpp nodeeditor qwt tinyxml
        for (item, items) {
            $$compiler($$group($${COMPILERS_HEADER}.include.$$item), \
                $$copydir($$jframe_dir/include/3rdpart/$$item/*, $$this_dir/include/3rdpart/$$item/))
            $$compiler($$group($${COMPILERS_HEADER}.lib.$$item), \
                $$copylib($$jframe_dir/lib/3rdpart, $$item, $$this_dir/lib/3rdpart/, true))
        }

        DEFINES *= $$DEFINES_TEMP

    } else {
        warning(datastudio_dir is not exists!)
    }

} else {

    COMPILERS_HEADER = $${ROOT}.$${TARGET}.header

}

#-------------------------------------------------

contains(SETENV_FLAGS, MODULE_3RDPART) {

    ## -- this - 3rdpart
    items = jchart jwt log4cpp qtribbon qwt tinyxml jutraledit jencrypt
    for (item, items) {
        name = $$replace(item, \*, _)
        $$compiler($$group($${ROOT}.3rdpart.$$name), \
            $$copydll($$this_dir/lib/3rdpart, $${item}, $$this_dir/bin/))
    }
    ## -- this - 3rdpart - moxa
    items = pcomm
    for (item, items) {
        name = $$replace(item, \*, _)
        $$compiler($$group($${ROOT}.3rdpart.$$name), \
            $$copydll($$this_dir/lib/3rdpart/moxa, $${item}, $$this_dir/bin/))
    }
}

#-------------------------------------------------

contains(SETENV_FLAGS, MODULE_CORE) {
    # -- Qt - 3rdpart
    items = d3dcompiler_47 libEGL libGLESv2 opengl32sw
    for (item, items) {
        $$compiler($$group($${ROOT}.core.$$item), \
            $$copydll($$QTDIR/bin, $${item}, $$this_dir/bin/))
    }
    # -- Qt - core
    items = Concurrent Core Gui Network PrintSupport SerialPort \
        Svg Widgets Xml
    for (item, items) {
        name = Qt$${QT_MAJOR_VERSION}$${item}
        $$compiler($$group($${ROOT}.core.$$name), \
            $$copydll($$QTDIR/bin, $${name}, $$this_dir/bin/))
    }
    # -- Qt - plugins
    items = bearer iconengines imageformats platforms translations
    for (item, items) {
        $$compiler($$group($${ROOT}.core.$$item), \
            $$copydir($$QTDIR/plugins/$${item}/*, $$this_dir/bin/$${item}/))
    }
    ## -- this - core
    items = icdchart icdcomm icdcore icdparser icdwidget icdworker
    for (item, items) {
        name = $$replace(item, \*, _)
        $$compiler($$group($${ROOT}.core.$$name), \
            $$copydll($$this_dir/lib/core, $${item}, $$this_dir/bin/))
    }
}
