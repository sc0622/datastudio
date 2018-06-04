import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import qbs.Environment
import tools.EnvUtils

PackageProduct {

    property stringList module3rdpart: []

    Group {
        name: 'data-3rdpart'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart') + '/'
        files: {
            var files = [];
            module3rdpart.forEach(function(item){
                files.push(item + EnvUtils.dylibSuffix(qbs));
            });
            return files;
        }
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
    }
}
