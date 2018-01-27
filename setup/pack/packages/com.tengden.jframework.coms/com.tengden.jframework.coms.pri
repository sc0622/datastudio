#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    items = jcom_layout jcom_login
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copyComponent($$jframe_dir/component/jframe/$${item}, $${item}, \
                $$PWD/data/jframe/$${item}/))
    }
}
