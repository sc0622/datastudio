import qbs
import qbs.File
import qbs.FileInfo

PackageProduct {

    property string module: ''

    Group {
        name: 'dynamic.files'
        prefix: FileInfo.joinPaths(projectDir, 'component', module, targetName) + '/'
        files: {
            var files = [];
            // dynamic
            var fileName = targetName + project.variantSuffix + '.dll';
            if (File.exists(prefix + '/' + fileName)) {
                files.push(fileName);
            }
            // xml
            fileName = targetName + '.xml';
            if (File.exists(prefix + '/' + fileName)) {
                files.push(fileName);
            }
            return files;
        }
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: FileInfo.joinPaths(module, targetName)
    }
}
