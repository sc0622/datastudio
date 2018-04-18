import qbs
import qbs.File
import qbs.FileInfo

PackageProduct {
    productPath: path

    // 3rdpart
    Group {
        name: 'data-core'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', 'core') + '/'
        files: {
            var files = [];
            [ 'icdchart', 'icdcomm', 'icdcore', 'icdparser', 'icdwidget', 'icdworker' ]
            .forEach(function(item){
                  files.push(item + project.variantSuffix + '.dll');
              });
            return files;
        }
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
    }
}
