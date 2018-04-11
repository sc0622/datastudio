import qbs
import qbs.FileInfo

Module {
    Depends { name: 'cpp' }
    Depends { name: 'Qt.core' ; cpp.link: false }

    readonly property path sourcePath: FileInfo.joinPaths(product.sourceDirectory, 'data')
    readonly property path targetPath: FileInfo.joinPaths(project.completeSetupDir, 'packages',
                                                          product.domain, 'data')
    readonly property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages',
                                                               product.domain, 'data')
}
