#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()
NAME = editor

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    ## -- config
    item = $${NAME}.config
    name = $$replace(item, \., '_')
    $$compiler($$group($${ROOT}.$$name), \
        $$copydir($$this_dir/config/$${NAME}/*, $$PWD/data/config/$${NAME}/, \
            $$shell_path("$$PWD/copy.ignore")))
    ## -- application
    item = $${NAME}
    $$compiler($$group($${ROOT}.$$item), \
        $$copyfile($$this_dir/bin/$${item}$$execsuffix(), $$PWD/data/bin/))
    ## -- component
    items = DataManagement ICDMainView ICDNavigation
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copyComponent($$this_dir/component/$${NAME}/$${item}, $${item}, \
                $$PWD/data/component/$${NAME}/$${item}/))
    }
}
