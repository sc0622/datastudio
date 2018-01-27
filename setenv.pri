#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:18:11
#
#-------------------------------------------------

ROOT = $$section(PWD, '/', -1)
INPUT =
QT_DIR = $$getenv(QTDIR)

exists($$PWD/*.ignore):DISTFILES += $$PWD/*.ignore

#-------------------------------------------------

$$compiler($$group($${ROOT}.$${TARGET}.header), $$group(echo --- $$PWD))

#-------------------------------------------------

## core
items = icdchart icdcore icdcomm icdparser icdworker icdwidget DBAccess KernelClass \
    icdcoremeta jencrypt
for (item, items) {
    $$compiler($$group($${ROOT}.$${TARGET}.$${item}), \
        $$copydll($$this_dir/lib/core, $${item}, $$this_dir/bin/core/))
}
