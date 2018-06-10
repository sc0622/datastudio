import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import qbs.Environment

Project {

    QtInstall {
        name: 'setenv-qt'
        moduleGeneral: base.concat([ 'Qml', 'SerialPort', 'Sql' ])
        modulePlugins: base.concat([ 'sqldrivers' ])
    }

    JFrameworkInstall {
        id: setenv_jframework
        name: 'setenv-jframework'
        condition: !project.buildQuote
        module3rdpart: [
            'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp', 'qwt',
            'tinyxml', 'qtftp', 'qttelnet'
        ]
    }

    DataStudioInstall {
        name: 'setenv-datastudio'
        condition: !project.buildQuote
        moduleCore: [
            'icdcomm', 'icdcore', 'icdparser', 'icdwidget', 'icdworker'
        ]
        installMeta: true
        installMoxa: true
    }

    Product {
        name: 'setenv'
        Depends { name: 'setenv-qt' }
        Depends { name: 'setenv-jframework'; required: false }
        Depends { name: 'setenv-datastudio'; required: false }
    }
}
