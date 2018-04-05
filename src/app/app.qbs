import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.ModUtils

Project {

    readonly property path langPath: FileInfo.joinPaths(path, 'resource', 'lang')

    Product {
        name: 'lupdate'
        type: [ 'translation' ]
        readonly property path currentPath: path

        Rule {
            multiplex: true
            Artifact { fileTags: [ 'translation' ] }
            prepare: {
                var cmd = new Command('lupdate', [/*'-silent', */'-recursive', product.currentPath, '-ts',
                                                  FileInfo.joinPaths(project.langPath, 'app_zh_CN.ts')]);
                cmd.description = 'generating translation file';
                return [ cmd ];
            }
        }
    }

    Product {
        name: 'lrelease'
        condition: false
        type: [ 'ts' ]
        Depends { name: 'Qt.core' }
        Depends { name: 'lupdate'; required: false; cpp.link: false }
        files: [ 'resource/lang/*.ts' ]
        destinationDirectory: project.langPath
    }

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

        Depends { name: 'lrelease'; required: false; cpp.link: false }

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
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }
    }
}
