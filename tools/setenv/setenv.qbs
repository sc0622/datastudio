import qbs
import qbs.FileInfo
import 'setenv.js' as Env

Project {

    QtInstall {
        name: 'setenv-qt'
        modulesBase: base.concat([ 'SerialPort' ])
    }

    JFrameworkInstall {
        name: 'setenv-jframework'
        modules3rdpart: [
            'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp', 'qwt', 'tinyxml'
        ]
    }

    Product {
        name: 'setenv'
        Depends { name: 'setenv-qt' }
        Depends { name: 'setenv-jframework' }

        // 3rdpart - moxa

        Group {
            name: 'core_moxa'
            prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart', 'moxa') + '/'
            files: [ 'pcomm.dll' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }
    }
}
