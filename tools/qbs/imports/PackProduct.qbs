import qbs
import qbs.File
import qbs.FileInfo
import '../../setenv/setenv.js' as Func

Product {

    Depends { name: 'cpp'; cpp.link: false }

    readonly property string domain: FileInfo.relativePath(sourceDirectory + '/..',
                                                           sourceDirectory)
    readonly property path jframeDir: Func.jframeDir(project)
    readonly property string projectDir: FileInfo.joinPaths(qbs.installRoot, project.projectName)
    readonly property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
    readonly property string installDir: FileInfo.joinPaths(domain, 'data')
}
