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
    Depends { name: 'qtribbon' }
    Depends { name: 'jframe_facade' }
    Depends { name: 'jframe_factory' }
    Depends { name: 'jframe_kernel' }

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
        files: [ 'resource/**/*.qrc' ]
    }
}
