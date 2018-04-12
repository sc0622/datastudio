import qbs
import qbs.File
import qbs.FileInfo

CppApp {

    type: base.concat([ 'translation' ])

    Depends { name: "Qt.gui" }
    Depends { name: 'Qt.widgets' }

    property bool linkDefaultQpaPlugin: Qt.gui.isStaticLibrary

    // translation
    property path langPath: FileInfo.joinPaths(sourceDirectory, 'resource', 'lang')
    property pathList noRecursivePaths: []
    property pathList recursivePaths: []
    property stringList translationFileTags: [ 'hpp', 'cpp' ]
    property stringList translations: []
    property bool defaultTranslation: false

    targetName: name
    desc.condition: true
    desc.fileDesc: 'Smartsoft® Application'
    desc.version: version
    desc.productName: targetName

    Rule {
        condition: defaultTranslation && translations.length > 0
        multiplex: true
        inputs: translationFileTags
        Artifact { fileTags: [ 'translation' ] }
        prepare: {
            var args = [];
            // no-recursive
            if (product.noRecursivePaths.length > 0) {
                args.push('-no-recursive');
                product.noRecursivePaths.forEach(function(item){
                    args.push(item);
                });
            }
            // recursive
            args.push('-recursive');
            if (product.recursivePaths.length > 0) {
                product.recursivePaths.forEach(function(item){
                    args.push(item);
                });
            } else {
                args.push(product.sourceDirectory);
            }
            // ts - files
            if (product.translations.length > 0) {
                args.push('-ts');
                product.translations.forEach(function(item){
                    args.push(FileInfo.joinPaths(product.langPath, item));
                });
            }
            // create command
            var cmd = new Command(product.Qt.core.binPath + '/lupdate', args);
            cmd.description = 'generating translation file...';
            return [ cmd ];
        }
    }
}
