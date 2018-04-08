import qbs
import qbs.FileInfo
import qbs.Environment

Project {

    readonly property string datastudioDomain: 'com.smartsoft.datastudio'
    readonly property string setupDir: 'setup'
    readonly property path completeSetupDir: FileInfo.joinPaths(qbs.installRoot, setupDir)
    readonly property string ifwDir: FileInfo.fromWindowsSeparators(Environment.getEnv('QTIFW_DIR'))
    readonly property path packagesPath: path + '/modules/packages';

    //qbsSearchPaths: [ '.' ]

//    Group {
//        name: 'modules'
//        //files: 'modules/**/*'
//    }

    //Depends { name: '.config' }
    //Depends { name: '.packages.' + datastudioDomain + '.3rdpart' }
    //Depends { name: '.packages.' + datastudioDomain + '.core' }
    //Depends { name: '.packages.' + datastudioDomain + '.app' }

    Product {
        name: 'pack-build'
        Depends { name: 'package_3rdpart' }
    }

    references: [
        'modules/packages/com/smartsoft/datastudio/3rdpart/module.qbs'
    ]
}
