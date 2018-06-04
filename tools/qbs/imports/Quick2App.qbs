import qbs
import qbs.File
import qbs.FileInfo

CppApp {

    type: base.concat([ 'translation' ])

    Depends { name: 'Qt.gui' }
    Depends { name: 'Qt.network' }
    Depends { name: 'Qt.qml' }
    Depends { name: 'Qt.quick' }
    Depends { name: 'Qt.quickcontrols2' }
    Depends {
        name: "Qt"
        submodules: Qt.gui.defaultQpaPlugin
        condition: linkDefaultQpaPlugin && Qt.gui.defaultQpaPlugin
    }

    property bool linkDefaultQpaPlugin: Qt.gui.isStaticLibrary

    // translation
    property path langPath: sourceDirectory + '/resource/lang'
    property pathList noRecursivePaths: []
    property pathList recursivePaths: []
    property stringList translationFileTags: [ 'hpp', 'cpp' ]
    property stringList translations: []
    property bool defaultTranslation: false
    // Additional import path used to resolve QML modules in Qt Creator's code model
    property pathList qmlImportPaths: [];

    Qt.quick.qmlDebugging: Qt.core.qtBuildVariant == 'debug'
    cpp.cxxLanguageVersion: "c++11"

    targetName: name + (qbs.buildVariant == 'debug' ? 'd' : '')
    desc.fileDesc: 'Tengden® Application'

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
                })
            }
            args.push('-recursive');
            // recursive
            if (product.recursivePaths.length > 0) {
                product.recursivePaths.forEach(function(item){
                    args.push(item);
                })
            } else {
                args.push(product.sourceDirectory);
            }
            // ts
            if (product.translations.length > 0) {
                args.push('-ts');
                product.translations.forEach(function(item){
                    args.push(product.langPath + '/' + item);
                })
            }
            // create command
            var cmd = new Command(product.Qt.core.binPath + '/lupdate', args);
            cmd.description = 'generating translation file...';
            return [ cmd ];
        }
    }
}
