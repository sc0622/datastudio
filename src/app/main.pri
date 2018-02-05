
this_dir = $$absolute_path($$PWD/../..)

include($$this_dir/src/common/build.pri)
include($$PWD/common/common.pri)

#-------------------------------------------------
# global commands
#-------------------------------------------------
#CONFIG(debug, debug|release):DEFINES += DEVELOP

#DEFINES *= SETENV

include($$this_dir/tools/function.pri)

contains(DEFINES, SETENV) {
    include($$PWD/setenv.pri)
} else {
    #warning(SETENV is not defined!)
}

ROOT = $$section(PWD, '/', -1)
INPUT = $$PWD/resource/image/app.ico

win32|unix: {
    ##
    $$compiler($$group($${ROOT}.header), $$group(echo --- console - $$TARGET ---))

    # core
    suffixes = exp ilk lib
    for (suffix, suffixes) {
        item = $${TARGET}*.$${suffix}
        name = $$replace(item, \*|\., '_')
        $$compiler($$group($${ROOT}.$${name}), $$removefile($$this_dir/bin/$${item}?))
    }
}
