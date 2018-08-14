import qbs
import qbs.File
import qbs.FileInfo

CoreDyLibrary {
    type: base.concat([ 'qmlfiles.out' ])
    translations: [ 'zh_CN.ts' ]
    defaultTranslation: true
    Qt.core.resourceFileBaseName: 'resource'

    Depends { name: 'Qt.widgets' }
    Depends { name: 'Qt.concurrent' }
    Depends { name: 'Qt.xml' }
    Depends { name: 'jwt'; required: false }
    Depends { name: '3rdpart.jwt'; required: false }
    Depends { name: 'icdcomm' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdworker' }
    Depends { name: 'icdparser' }

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
        prefix: 'resource/'
        excludeFiles: [ '**/*.ts', '**/*.psd' ]
        files: [ '**/*' ]
        fileTags: [ 'qt.core.resource_data' ]
        Qt.core.resourcePrefix: '/icdwidget'
        Qt.core.resourceSourceBase: 'resource'
    }

    Probe {
        id: probeQmlFiles
        readonly property path qtTrPath: Qt.core.binPath + '/../translations/'
        readonly property path sourceDirectory: product.sourceDirectory
        configure: {
            var qmlfiles = [ 'qt_zh_CN', 'qt_help_zh_CN', 'qtbase_zh_CN' ]
            var targetDir = sourceDirectory + '/resource/lang/';
            qmlfiles.forEach(function(file){
                var file2 = file + '.qm';
                var source = qtTrPath + file2;
                if (File.exists(source)) {
                    File.copy(source, targetDir + file2);
                }
            })
            found = true;
        }
    }
}
