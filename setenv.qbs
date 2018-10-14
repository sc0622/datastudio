import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import qbs.Environment

Project {

    QtInstall {
        condition: qbs.targetOS.contains('window') //TODO [fix other platform]
        name: 'setenv-qt'
        moduleGeneral: base.concat([ 'Qml', 'SerialPort', 'Sql' ])
        modulePlugins: base.concat([ 'sqldrivers' ])
    }

    JFrameworkInstall {
        condition: !project.buildQuote
        id: setenv_jframework
        name: 'setenv-jframework'
        module3rdpart: [
            'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp', 'qwt',
            'tinyxml', 'qtftp', 'qttelnet'
        ]
    }

    DataStudioInstall {
        condition: !project.buildQuote && !qbs.targetOS.contains('darwin') //TODO [fix other platform]
        name: 'setenv-datastudio'
        moduleCore: [
            'icdcomm', 'icdcore', 'icdparser', 'icdwidget', 'icdworker'
        ]
        installMeta: true
        installMoxa: true
    }

    Product {
        name: 'setenv'
        Depends { name: 'setenv-qt'; required: false }
        Depends { name: 'setenv-jframework'; required: false }
        Depends { name: 'setenv-datastudio'; required: false }
    }
}
