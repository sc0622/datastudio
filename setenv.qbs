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
    }

    Product {
        name: 'setenv'
        Depends { name: 'setenv-qt' }
        Depends { name: 'setenv-jframework'; required: false }
        Depends { name: 'setenv-datastudio'; required: false }

        // 3rdpart - self

        Group {
            name: 'dynamic_3rdpart_self'
            prefix: project.sourceDirectory + '/lib/3rdpart/'
            files: [ 'moxa/pcomm' + (qbs.architecture == 'x86_64' ? '_x86_64' : '') + '.dll' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }
    }
}
