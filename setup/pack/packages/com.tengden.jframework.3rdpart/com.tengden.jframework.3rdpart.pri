#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    items = jchart jutraledit jwt log4cpp qtribbon qtwinmigrate qwt tinyxml \
        jencrypt
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copydll($$jframe_dir/bin/3rdpart, $${item}, $$PWD/data/3rdpart/))
    }
}
