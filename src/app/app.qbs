import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.ModUtils

Project {

    WidgetApp {
        id: app

        consoleApplication: false
        targetName: 'datastudio'
        version: project.version

        translations: [ 'app_zh_CN.ts' ]
        defaultTranslation: true

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

        cpp.defines: base.concat([ 'PROJECT_APP' ])
        cpp.includePaths: base.concat([ '.' ])

        desc.iconName: 'resource/image/app.ico'

        Group {
            fileTagsFilter: [ 'application' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }
    }

    Product {
        name: 'lrelease'
        condition: false
        type: [ 'ts' ]
        Depends { name: 'Qt.core' }
        files: [ 'resource/lang/*.ts' ]
        destinationDirectory: app.langPath
    }
}
