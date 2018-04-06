import qbs
import qbs.FileInfo

QtGuiApplication {
    version: '1.0.0'

    type: base.concat([ 'translation' ])

    // module
    readonly property path precompPath: project.sourceDirectory + '/src/common'

    // translation
    property path langPath: FileInfo.joinPaths(sourceDirectory, 'resource', 'lang')
    property stringList translationFileTags: [ 'hpp', 'cpp' ]
    property stringList translations: []
    property bool defaultTranslation: false

    Depends { name: 'Qt.core' }
    Depends { name: 'Qt.widgets' }
    Depends { name: "Qt.qminimal"; condition: Qt.core.staticBuild }
    Depends { name: 'desc'; required: false }

    targetName: name
    desc.condition: true
    desc.fileDesc: 'Smartsoft® Application'
    desc.version: version
    desc.productName: targetName

    cpp.includePaths: base.concat([precompPath,
                                   project.sourceDirectory + '/include',
                                   project.sourceDirectory + '/include/3rdpart',
                                   project.sourceDirectory + '/include/core'
                                  ])
    cpp.libraryPaths: base.concat([project.sourceDirectory + '/lib',
                                   project.sourceDirectory + '/lib/3rdpart',
                                   project.sourceDirectory + '/lib/core'])
    cpp.variantSuffix: project.variantSuffix

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

    // rc
    Group {
        name: 'windres'
        condition: desc.condition && qbs.targetOS.contains('windows')
        // We need the version in two separate formats for the .rc file
        //  RC_VERSION=1,0,0,0 (quadruple)
        //  RC_VERSION_STRING='1.0.0-xxx' (free text)
        // Also, we need to replace space with \x20 to be able to work with both rc and windres
        cpp.defines: {
            var defines = outer.concat(['RC_ICON1=' + desc.iconName,
                                        'RC_FILEDESC=' + desc.fileDesc.replace(/ /g, '\\x20'),
                                        'RC_PRODUCTNAME=' + desc.productName.replace(/ /g, '\\x20'),
                                        'RC_COMPANYNAME=' + desc.companyName.replace(/ /g, '\\x20'),
                                        'RC_VERSION=' + desc.version.replace(/\./g, ',') + ',0',
                                        'RC_VERSION_STRING=' + desc.displayVersion,
                                        'RC_COPYRIGHT=2016-' + desc.copyrightYear
                                        + ' ' + desc.companyName.replace(/ /g, '\\x20')])
            if (desc.iconName.length > 0) {
                defines.push('GENERATE_ICON');
            }
            return defines;
        }
        files: 'resource/windows/application.rc'
    }
}
