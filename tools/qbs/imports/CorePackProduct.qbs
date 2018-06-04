import qbs
import qbs.File
import qbs.FileInfo

PackageProduct {

    property string module: ''
    property string variantSuffix: project.variantSuffix

    Group {
        name: 'dynamic.files'
        prefix: FileInfo.joinPaths(projectDir, 'bin/core', module) + '/'
        files: {
            var files = [];
            // dynamic
            var fileName = targetName + variantSuffix + '.dll';
            if (File.exists(prefix + '/' + fileName)) {
                files.push(fileName);
            }
            return files;
        }
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: FileInfo.joinPaths('core', module)
    }
}
