import qbs
import qbs.File
import qbs.FileInfo

CppDyLibrary {
    module: 'component'
    defaultCopyHeader: false
    defaultExport: false
    installPrefix: module
    installDir: name

    type: base.concat([ 'config' ])

    Depends { name: 'cpp' }
    Depends { name: 'qtribbon'; required: false }
    Depends { name: 'jframe_facade'; required: false }
    Depends { name: 'jframe_factory'; required: false }
    Depends { name: 'jframe_kernel'; required: false }

    Depends { name: '3rdpart.qtribbon'; required: false }
    Depends { name: 'core.jframe_facade'; required: false }
    Depends { name: 'core.jframe_factory'; required: false }
    Depends { name: 'core.jframe_kernel'; required: false }

    Group {
        name: 'Config'
        prefix: product.sourceDirectory + '/'
        files: [ '*.xml' ]
        fileTags: [ 'config.in' ]
        qbs.install: defaultInstall
        qbs.installPrefix: project.projectName
        qbs.installDir: FileInfo.joinPaths(installPrefix, installDir)
    }

    Group {
        name: 'Resources'
        prefix: product.sourceDirectory + '/'
        files: [ 'resource/**/*.qrc' ]
    }
}
