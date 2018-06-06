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

        readonly property bool useOldEdit: true

        Depends { name: 'Qt.concurrent' }
        Depends { name: 'Qt.network' }
        Depends { name: 'Qt.sql'; linked: product.useOldEdit }
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
            excludeFiles: [ '**/precomp.h', 'common/old/**/*', 'module/edit/old/**/*' ]
        }

        Group {
            name: 'Sources'
            files: [ '**/*.cpp' ]
            excludeFiles: [ 'common/old/**/*', 'module/edit/old/**/*' ]
        }

        Group {
            name: 'Resources'
            files: [ 'resource/*.qrc' ]
        }
        cpp.defines: {
            var defines = base.concat(['PROJECT_APP']);
            if (qbs.buildVariant == 'debug') {
                defines.push('TEST_STYLESHEET');
            }
            // old
            if (useOldEdit) {
                defines.push('EDIT_OLD');
            }
            return defines;
        }
        cpp.includePaths: {
            var paths = base.concat(['.']);
            if (useOldEdit) {
                paths.push('common/old');
                paths.push('common/old/DBAccess/parser');
                paths.push('module/edit/old');
            }
            return paths;
        }

        // old

        Group {
            name: 'old.DBAccess'
            condition: useOldEdit
            prefix: 'common/old/DBAccess/'
            files: [ '**/*.h', '**/*.cpp' ]
        }

        Group {
            name: 'old.KernelClass'
            condition: useOldEdit
            prefix: 'common/old/KernelClass/'
            files: [ '**/*.h', '**/*.cpp' ]
        }

        Group {
            name: 'old.ui'
            condition: useOldEdit
            prefix: 'module/edit/old/'
            files: [ '**/*.h', '**/*.cpp' ]
        }
    }
}
