import qbs
import qbs.FileInfo
import qbs.Utilities

CoreDyLibrary {
    condition: Utilities.versionCompare(Qt.core.version, '5.6.0') >= 0
    translationFileTags: base.concat([ 'qml', 'js' ])
    translations: [ 'icdmeta_zh_CN.ts' ]
    defaultTranslation: true
    defaultInstall: false

    Depends { name: 'Qt.qml' }
    Depends { name: 'Qt.quick' }
    Depends { name: 'Qt.quickcontrols2' }
    Depends { name: 'Qt.serialport' }
    Depends { name: 'Qt.concurrent' }
    Depends { name: '3rdpart.jencrypt' }
    Depends { name: '3rdpart.jchart' }
    Depends { name: '3rdpart.log4cpp' }
    Depends { name: '3rdpart.qtftp' }
    Depends { name: 'icdcomm' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdparser' }
    Depends { name: 'icdworker' }
    Depends { name: 'icdwidget' }

    Group {
        name: 'Headers'
        files: [ '**/*.h' ]
        excludeFiles: [ '**/private/**/*.h', '**/*_p.h' ]
        fileTags: ['hpp.in' ]
    }

    Group {
        name: 'Headers_private'
        files: [ '**/private/**/*.h', '**/*_p.h' ]
    }

    Group {
        name: 'Sources'
        files: [ '**/*.cpp' ]
    }

    Group {
        name: 'Resources'
        files: [ 'resource/*.qrc' ]
    }

    cpp.dynamicLibraries: base.concat([ 'ws2_32', 'advapi32', 'user32', 'gdi32' ])
}
