import qbs
import qbs.File
import qbs.FileInfo
import '../../setenv/setenv.js' as Func

Product {
    name: domain

    Depends { name: 'cpp'; cpp.link: false }

    readonly property string domain: FileInfo.relativePath(sourceDirectory + '/..', sourceDirectory)
    readonly property path projectDir: FileInfo.joinPaths(project.projectInstallRoot, project.projectName)
    readonly property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
    readonly property string installDir: FileInfo.joinPaths(domain, 'data')
    readonly property path dataDir: FileInfo.joinPaths(project.completeSetupDir, 'packages', installDir)
}
