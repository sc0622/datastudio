import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.ModUtils

Project {

    WidgetApp {
        targetName: project.projectName + (qbs.buildVariant == 'debug' ? 'd' : '')
        version: project.version

        translations: [ 'app_zh_CN.ts' ]
        defaultTranslation: true
        desc.iconName: sourceDirectory + '/resource/image/app.ico'

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
            excludeFiles: [ '**/precomp.h' ]
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
    }
}
