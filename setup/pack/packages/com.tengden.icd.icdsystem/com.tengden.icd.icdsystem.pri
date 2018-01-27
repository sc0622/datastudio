#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    ## -- config
    item = icdsystem.config
    name = $$replace(item, \., '_')
    $$compiler($$group($${ROOT}.$$name), \
        $$copydir($$this_dir/config/icdsystem/*, $$PWD/data/config/icdsystem/, \
            $$shell_path("$$PWD/copy.ignore")))

    ## -- component - common*
    items = com_channelmgr com_databasemgr com_icdsystem com_resource com_utraledit
    for (item, items) {
        $$compiler($$group($${ROOT}.$$item), \
            $$copyComponent($$this_dir/component/common/$${item}, $${item}, \
                $$PWD/data/component/common/$${item}/))
    }
    ## -- application
    item = icdsystem
    $$compiler($$group($${ROOT}.$$item), \
        $$copyfile($$this_dir/bin/$${item}$$execsuffix(), $$PWD/data/bin/))

    ## -- dll
    item = jframeworkdir
    $$compiler($$group($${ROOT}.$$item), \
        $$copyfile($$jframe_dir/bin/$${item}$$dylibsuffix(), $$PWD/data/bin/))
    ## -- data
    items = Default.xml Default.json
    for (item, items) {
        name = $$replace(item, \., '_')
        $$compiler($$group($${ROOT}.$$name), \
            $$copyfile($$this_dir/bin/data/$${item}, $$PWD/data/bin/data/))
    }
    ## -- msvc*
    win32 {
        items = msvcp* msvcr* vccorlib* vcruntime* concrt*
        for (item, items) {
            name = $$replace(item, \*, '_')
            $$compiler($$group($${ROOT}.$$name), \
                $$copydll($$group($$getenv(VCINSTALLDIR)redist/x86/Microsoft.VC140.CRT), \
                    $${item}, $$PWD/data/bin/))
        }
    }
}

