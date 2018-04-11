import qbs
import qbs.File
import qbs.FileInfo

Product {
    condition: !!productPath
    name: FileInfo.relativePath(productPath + '/..', productPath).replace(/\./g, '_')

    Depends { name: 'cpp' }
    Depends { name: 'pack.data'; cpp.link: false }
    Depends { name: 'pack.meta'; cpp.link: false }

    property path productPath
    readonly property path qtdir: FileInfo.cleanPath(Qt.core.binPath + '/..')
    readonly property path projectDir: FileInfo.joinPaths(project.projectInstallRoot, project.projectName)
    readonly property string domain: FileInfo.relativePath(sourceDirectory + '/..', sourceDirectory)
    readonly property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
    readonly property path metaSourcePath: pack.meta.sourcePath
    readonly property path metaTargetPath: pack.meta.targetPath
    readonly property string dataInstallPrefix: pack.data.installPrefix
    readonly property string metaInstallPrefix: pack.meta.installPrefix
}
