#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    ## -- config
    item = config
    name = $$replace(item, \., '_')
    $$compiler($$group($${ROOT}.$$name), \
        $$copydir($$this_dir/config/*, $$PWD/data/config/, \
            $$shell_path("$$PWD/copy.ignore")))

    ## -- application
    item = ModelSimu
    $$compiler($$group($${ROOT}.$$item), \
        $$copyfile($$this_dir/bin/$${item}$$execsuffix(), $$PWD/data/bin/))

    ## -- tools
    win32 {
        items = rasshost.exe BCGCBPRO180090.dll
        for (item, items) {
            name = $$replace(item, \., _)
            $$compiler($$group($${ROOT}.$$name), \
                $$copyfile($$this_dir/test/rass/$${item}, $$PWD/data/bin/tools/))
        }
    }

    ## -- msvc*
    win32 {
        items = msvcp* msvcr* vccorlib
        for (item, items) {
            name = $$replace(item, \*, '_')
            $$compiler($$group($${ROOT}.$$name), \
                $$copydll($$group($$getenv(VCINSTALLDIR)redist/x86/Microsoft.VC100.CRT), \
                    $${item}, $$PWD/data/bin/))
        }
    }
}

DISTFILES += \
    $$PWD/meta/*.*
