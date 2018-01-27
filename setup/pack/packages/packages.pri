#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T18:09:05
#
#-------------------------------------------------

include($$PWD/com.tengden.icd.3rdpart/com.tengden.icd.3rdpart.pri)
include($$PWD/com.tengden.icd.core/com.tengden.icd.core.pri)
include($$PWD/com.tengden.icd.core.DBAccess/com.tengden.icd.core.DBAccess.pri)
include($$PWD/com.tengden.icd.core.KernelClass/com.tengden.icd.core.KernelClass.pri)
include($$PWD/com.tengden.icd.core.icdchart/com.tengden.icd.core.icdchart.pri)
include($$PWD/com.tengden.icd.core.icdcomm/com.tengden.icd.core.icdcomm.pri)
include($$PWD/com.tengden.icd.core.icdcore/com.tengden.icd.core.icdcore.pri)
include($$PWD/com.tengden.icd.core.icdparser/com.tengden.icd.core.icdparser.pri)
include($$PWD/com.tengden.icd.core.icdwidget/com.tengden.icd.core.icdwidget.pri)
include($$PWD/com.tengden.icd.core.icdworker/com.tengden.icd.core.icdworker.pri)
include($$PWD/com.tengden.icd.icdsystem/com.tengden.icd.icdsystem.pri)

contains(DEFINES, ICD_EDITOR) {
    include($$PWD/com.tengden.icd.icdsystem.editor/com.tengden.icd.icdsystem.editor.pri)
} else {
    PKG_EXCLUDE += com.tengden.icd.icdsystem.editor
    !contains(DEFINES, PACKAGE) {
        include($$PWD/com.tengden.icd.icdsystem.editor/com.tengden.icd.icdsystem.editor.pri)
    }
}

contains(DEFINES, ICD_MONITOR) {
    include($$PWD/com.tengden.icd.icdsystem.monitor/com.tengden.icd.icdsystem.monitor.pri)
} else {
    PKG_EXCLUDE += com.tengden.icd.icdsystem.monitor
    !contains(DEFINES, PACKAGE) {
        include($$PWD/com.tengden.icd.icdsystem.monitor/com.tengden.icd.icdsystem.monitor.pri)
    }
}

contains(DEFINES, ICD_SIMULATOR) {
    include($$PWD/com.tengden.icd.icdsystem.simulator/com.tengden.icd.icdsystem.simulator.pri)
} else {
    PKG_EXCLUDE += com.tengden.icd.icdsystem.simulator
    !contains(DEFINES, PACKAGE) {
        include($$PWD/com.tengden.icd.icdsystem.simulator/com.tengden.icd.icdsystem.simulator.pri)
    }
}

contains(DEFINES, ICD_ANALYSE) {
    include($$PWD/com.tengden.icd.icdsystem.analyse/com.tengden.icd.icdsystem.analyse.pri)
} else {
    PKG_EXCLUDE += com.tengden.icd.icdsystem.analyse
    !contains(DEFINES, PACKAGE) {
        include($$PWD/com.tengden.icd.icdsystem.analyse/com.tengden.icd.icdsystem.analyse.pri)
    }
}

#-------------------------------------------------

contains(DEFINES, JFRAMEWORK) {
    include($$PWD/com.tengden.jframework/com.tengden.jframework.pri)
    include($$PWD/com.tengden.jframework.core/com.tengden.jframework.core.pri)
    include($$PWD/com.tengden.jframework.3rdpart/com.tengden.jframework.3rdpart.pri)
    include($$PWD/com.tengden.jframework.3rdpart.qt/com.tengden.jframework.3rdpart.qt.pri)
    include($$PWD/com.tengden.jframework.coms/com.tengden.jframework.coms.pri)
} else {
    PKG_EXCLUDE += \
        com.tengden.jframework \
        com.tengden.jframework.core \
        com.tengden.jframework.3rdpart \
        com.tengden.jframework.3rdpart.qt \
        com.tengden.jframework.coms
    !contains(DEFINES, PACKAGE) {
        include($$PWD/com.tengden.jframework/com.tengden.jframework.pri)
        include($$PWD/com.tengden.jframework.core/com.tengden.jframework.core.pri)
        include($$PWD/com.tengden.jframework.3rdpart/com.tengden.jframework.3rdpart.pri)
        include($$PWD/com.tengden.jframework.3rdpart.qt/com.tengden.jframework.3rdpart.qt.pri)
        include($$PWD/com.tengden.jframework.coms/com.tengden.jframework.coms.pri)
    }
}

#-------------------------------------------------
