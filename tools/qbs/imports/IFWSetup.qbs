import qbs
import qbs.File
import qbs.FileInfo

Project {
    property path setupPath: null
    readonly property string setupDir: 'setup'
    readonly property path completeSetupDir: FileInfo.joinPaths(qbs.installRoot, setupDir)
    readonly property path packagesPath: setupPath ? setupPath + '/modules/packages' : null;

    references: {
        if (!project.projectInstallRoot) {
            return;
        }
        return [sourceDirectory + '/setup/config/config.qbs',
                sourceDirectory + '/setup/packages/packages.qbs']
    }

    Product {
        Depends { name: 'pack.genproj'; condition: !project.projectInstallRoot }
        Depends { name: 'pack.build'; condition: !!project.projectInstallRoot }
        pack.build.productName: project.setupName ? project.setupName : original
    }
}
