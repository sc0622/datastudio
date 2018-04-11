import qbs
import qbs.FileInfo

Module {
    id: root

    property bool enabledInstall: true
    readonly property path sourcePath: FileInfo.joinPaths(product.sourceDirectory, 'meta')
    readonly property path targetPath: FileInfo.joinPaths(project.completeSetupDir, 'packages',
                                                          product.domain, 'meta')
    property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages',
                                                      product.domain, 'meta')

    Depends { name: 'cpp' }

    additionalProductTypes: [ 'meta.out' ]

    Group {
        id: groupMeta
        name: 'meta'
        fileTags: [ 'meta.in' ]
        prefix: root.sourcePath + '/'
        files: [ '**/*' ]
        qbs.install: root.enabledInstall
        qbs.installPrefix: product.metaInstallPrefix
        qbs.installSourceBase: name
    }
}
