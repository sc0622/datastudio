import qbs
import qbs.FileInfo

QtGuiApplication {

    type: base.concat([ 'translation' ])

    readonly property path precompPath: project.sourceDirectory + '/src/common'
    property path langPath: FileInfo.joinPaths(sourceDirectory, 'resource', 'lang')
    property stringList translationFileTags: [ 'hpp', 'cpp' ]
    property stringList translations: []
    property bool defaultTranslation: false

    cpp.includePaths: base.concat([
                                      precompPath,
                                      project.sourceDirectory + '/include',
                                      project.sourceDirectory + '/include/3rdpart',
                                      project.sourceDirectory + '/include/core'
                                  ])

    targetName: name
    cpp.variantSuffix: project.variantSuffix

    Depends { name: 'Qt.widgets' }

    Group {
        name: 'Precomp'
        condition: cpp.useCxxPrecompiledHeader
        prefix: precompPath + '/'
        files: ['precomp.h']
        fileTags: ['cpp_pch_src']
    }

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
