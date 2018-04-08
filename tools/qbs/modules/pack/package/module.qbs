import qbs
import qbs.File
import qbs.FileInfo

Module {
    condition: !!path

    Depends { name: 'cpp' }
    Depends { name: 'pack.data'; cpp.link: false }
    Depends { name: 'pack.meta'; cpp.link: false }

    pack.meta.domain: domain

    property path path
    readonly property string domain: FileInfo.relativePath(product.packagesPath, path).replace(/\//g, '.')
    readonly property path projectDir: FileInfo.joinPaths(project.projectInstallRoot, project.projectName)
    readonly property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
    readonly property string installDir: FileInfo.joinPaths(domain, 'data')
    readonly property path dataDir: FileInfo.joinPaths(project.completeSetupDir, 'packages', installDir)
}
