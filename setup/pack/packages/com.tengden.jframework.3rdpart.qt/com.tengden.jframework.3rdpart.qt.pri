#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    # -- Qt - 3rdpart
    items = d3dcompiler_47 libEGL libGLESv2 opengl32sw
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copydll($$jframe_dir/bin/3rdpart/Qt, $${item}, $$PWD/data/Qt/))
    }
    # -- Qt - core
    items = Concurrent Core DBus Gui Network OpenGL PrintSupport \
        Qml Quick SerialPort Sql Widgets Xml
    for (item, items) {
        name = Qt$${QT_MAJOR_VERSION}$${item}
        $$compiler($$group($${ROOT}.$$name), \
            $$copydll($$jframe_dir/bin/3rdpart/Qt, $${name}, $$PWD/data/Qt/))
    }
    # -- Qt - plugins
    items = bearer imageformats platforms sqldrivers
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copydir($$jframe_dir/bin/3rdpart/Qt/plugins/$${item}/*, \
                $$PWD/data/Qt/plugins/$${item}/, "$$shell_path($$PWD/copy.ignore)"))
    }
    # -- Qt - qml
    # -- Qt - translations
}
