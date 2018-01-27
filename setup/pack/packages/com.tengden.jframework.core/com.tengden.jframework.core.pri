#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    items = jframe_facade jframe_factory jframe_kernel jframeworkenv
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copydll($$jframe_dir/bin/core, $${item}, $$PWD/data/))
    }
}
