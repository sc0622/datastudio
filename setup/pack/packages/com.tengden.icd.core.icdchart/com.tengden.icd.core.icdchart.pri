#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    item = icdchart
    $$compiler($$group($${ROOT}.$$item), \
        $$copylib($$this_dir/lib/core, $${item}, $$PWD/data/core/))
}
