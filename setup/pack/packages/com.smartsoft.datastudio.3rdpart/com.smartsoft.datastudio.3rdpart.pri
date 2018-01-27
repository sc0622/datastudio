#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

QTDIR = $$getenv(QTDIR)
!exists($$QTDIR):error("QTDIR is invalid")

#-------------------------------------------------

contains(DEFINES, PACKAGE) {

    # -- Qt - 3rdpart
    items = d3dcompiler_47 libEGL libGLESv2 opengl32sw
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copydll($$QTDIR/bin, $${item}, $$PWD/data/bin/))
    }
    # -- Qt - core
    items = Concurrent Core Gui Network PrintSupport \
        Qml Quick Svg Widgets WinExtras Xml
    for (item, items) {
        name = Qt$${QT_MAJOR_VERSION}$${item}
        $$compiler($$group($${ROOT}.$$name), \
            $$copydll($$QTDIR/bin, $${name}, $$PWD/data/bin/))
    }
    # -- Qt - plugins
    excludes = $$shell_path("$$PWD/copy.ignore")
    items = bearer iconengines imageformats platforms translations
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copydir($$QTDIR/plugins/$${item}/*.dll, $$PWD/data/bin/$${item}/, $${excludes}))
    }
    ## -- this - 3rdpart

    items = jchart jwt log4cpp qtribbon qwt tinyxml rass*_md
    for (item, items) {
        name = $$replace(item, \*, _)
        $$compiler($$group($${ROOT}.$$name), \
            $$copydll($$this_dir/lib/3rdpart, $${item}, $$PWD/data/bin/))
    }
}

DISTFILES += \
    $$PWD/meta/*.*
