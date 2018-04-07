import qbs
import qbs.File
import qbs.FileInfo

CppApp {

    type: base.concat([ 'translation' ])

    Depends { name: "Qt.gui" }
    Depends { name: 'Qt.widgets' }
    Depends {
        name: "Qt"
        submodules: Qt.gui.defaultQpaPlugin
        condition: linkDefaultQpaPlugin && Qt.gui.defaultQpaPlugin
    }
    //Depends { name: 'setenv-sync'; required: false; cpp.link: false } //

    property bool linkDefaultQpaPlugin: Qt.gui.isStaticLibrary

    // translation
    property path langPath: sourceDirectory + '/resource/lang'
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
            var args = ['-recursive', product.sourceDirectory, '-ts'];
            product.translations.forEach(function(item){
                args.push(FileInfo.joinPaths(product.langPath, item));
            });
            var cmd = new Command('lupdate', args);
            cmd.description = 'generating translation file...';
            return [ cmd ];
        }
    }
}
