import qbs
import qbs.FileInfo

Module {
    id: root

    property string domain
    property bool enabledInstall: true
    readonly property path sourcePath: FileInfo.joinPaths(product.sourceDirectory, 'meta')
    readonly property path targetPath: FileInfo.joinPaths(project.completeSetupDir, 'packages', domain, 'meta')

    Depends { name: 'cpp' }

    Group {
        id: groupMeta
        name: 'meta'
        fileTags: [ 'meta.in' ]
        prefix: root.sourcePath + '/'
        files: [ '**/*' ]
        qbs.install: root.enabledInstall
        qbs.installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
        qbs.installDir: FileInfo.joinPaths(root.domain, name)
        qbs.installSourceBase: name
    }
}
