import qbs
import qbs.File
import qbs.FileInfo

Module {

    Depends { name: 'pack.package'; cpp.link: false }

    // 3rdpart
//    Group {
//        name: 'data-core'
//        prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', 'core') + '/'
//        files: {
//            var files = [];
//            [ 'icdchart', 'icdcomm', 'icdcore', 'icdparser', 'icdwidget', 'icdworker' ]
//            .forEach(function(item){
//                  files.push(item + project.variantSuffix + '.dll');
//              });
//            return files;
//        }
//        fileTags: [ 'pack.in' ]
//        qbs.install: true
//        qbs.installPrefix: installPrefix
//        qbs.installDir: installDir
//    }
}
