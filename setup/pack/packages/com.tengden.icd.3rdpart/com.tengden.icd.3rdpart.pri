#-------------------------------------------------
# Project created by QtCreator 2016-08-31T18:09:05
#-------------------------------------------------

package_init()

#-------------------------------------------------

contains(DEFINES, PACKAGE) {
    item = pcomm
    $$compiler($$group($${ROOT}.$$item), \
        $$copydll($$this_dir/bin/3rdpart/moxa/, $${item}, $$PWD/data/3rdpart/moxa/))
}
