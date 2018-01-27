#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    item = jframeworkdir
    $$compiler($$group($${ROOT}.$$item), \
        $$copydll($$this_dir/bin, $${item}, $$PWD/data/bin/))
}
