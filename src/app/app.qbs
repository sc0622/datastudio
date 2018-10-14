import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.ModUtils

WidgetApp {
    targetName: project.projectName
    version: project.version

    Depends { name: '3rdpart.qwt' }

    translations: [ 'app_zh_CN.ts' ]
    defaultTranslation: true
    desc.iconName: sourceDirectory + '/resource/image/app.ico'
    desc.fileDesc: project.projectDisplayName
    Qt.core.resourceFileBaseName: project.projectName

    cpp.defines: {
        var defines = base.concat(['PROJECT_APP']);
        if (qbs.buildVariant == 'debug') {
            defines.push('TEST_STYLESHEET');
        }
        return defines;
    }
    cpp.includePaths: base.concat([ '.' ])

    //TEST
    cpp.rpaths: base.concat([project.sourceDirectory + '/lib/3rdpart',
                             project.sourceDirectory + '/lib/core'
                            ])

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
        files: [
            '**/*.h',
        ]
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
        Qt.core.resourcePrefix: '/' + project.projectName
        Qt.core.resourceSourceBase: 'resource'
    }
}
