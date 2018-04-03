import qbs
import qbs.File
import qbs.FileInfo

WidgetApp {
    consoleApplication: false
    targetName: 'datastudio'

    Depends { name: 'Qt.concurrent' }
    Depends { name: 'Qt.network' }
    Depends { name: 'Qt.serialport' }
    Depends { name: '3rdpart.jchart' }
    Depends { name: '3rdpart.jutraledit' }
    Depends { name: '3rdpart.log4cpp' }
    Depends { name: 'icdchart' }
    Depends { name: 'icdcomm' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdparser' }
    Depends { name: 'icdworker' }
    Depends { name: 'icdwidget' }

    Group {
        name: 'Headers'
        files: [ '**/*.h' ]
    }

    Group {
        name: 'Sources'
        files: [ '**/*.cpp' ]
    }

    Group {
        name: 'Resources'
        files: [ 'resource/*.qrc' ]
    }

    cpp.defines: base.concat([ 'PRO_APP' ])
    cpp.includePaths: base.concat([ '.' ])

    Group {
        fileTagsFilter: [ 'application' ]
        qbs.install: true
        qbs.installPrefix: project.installPrefix
        qbs.installDir: 'bin'
    }
}
