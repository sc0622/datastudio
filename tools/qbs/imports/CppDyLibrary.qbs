import qbs
import qbs.File
import qbs.FileInfo

DynamicLibrary {
    version: '1.0.0'

    type: base.concat([ 'translation' ])

    // module
    property string module: ''
    readonly property path includePath: project.sourceDirectory + '/include/'
                                        + module + '/' + name
    readonly property path precompPath: project.sourceDirectory + '/src/common'

    // translation
    property path langPath: FileInfo.joinPaths(sourceDirectory, 'resource', 'lang')
    property stringList translationFileTags: [ 'hpp', 'cpp' ]
    property stringList translations: []
    property bool defaultTranslation: false

    // default flags
    property string relativeDirectory: ''
    property bool defaultCopyHeader: true
    property bool defaultCopyDynamic: true

    // install
    property stringList installFileTags: [ 'dynamiclibrary' ]
    property bool defaultInstall: true
    property string installPrefix: 'bin'
    property string installDir: ''

    // export
    property bool defaultExport: true

    Depends { name: 'cpp' }
    Depends { name: 'desc'; required: false }

    targetName: name
    desc.condition: true
    desc.fileDesc: 'Smartsoft® Runtime Library'
    desc.version: version
    desc.productName: targetName

    cpp.includePaths: base.concat([precompPath,
                                   project.sourceDirectory + '/include',
                                   project.sourceDirectory + '/include/3rdpart',
                                   project.sourceDirectory + '/include/core'])
    cpp.libraryPaths: base.concat([project.sourceDirectory + '/lib',
                                   project.sourceDirectory + '/lib/3rdpart',
                                   project.sourceDirectory + '/lib/core'])
    cpp.defines: {
        var upperName = product.name.toUpperCase();
        var defines = base;
        defines.push(upperName + '_LIB');
        defines.push(upperName + '_BUILD');
        return defines;
    }
    cpp.variantSuffix: project.variantSuffix

    Group {
        fileTagsFilter: installFileTags
        qbs.install: defaultInstall
        qbs.installPrefix: project.projectName
        qbs.installDir: FileInfo.joinPaths(installPrefix, installDir)
    }

    Group {
        name: 'Precomp'
        condition: cpp.useCxxPrecompiledHeader
        prefix: precompPath + '/'
        files: ['precomp.h']
        fileTags: ['cpp_pch_src']
    }

    Rule {
        condition: product.defaultCopyHeader
        inputs: [ 'hpp.in' ]
        Artifact {
            filePath: product.includePath  + '/' + FileInfo.relativePath(
                          FileInfo.joinPaths(product.sourceDirectory, product.relativeDirectory),
                          input.filePath)
            fileTags: [ 'hpp' ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = 'coping ' + input.fileName;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
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

    Export {
        condition: product.defaultExport
        Depends { name: 'cpp' }
        cpp.defines: [ product.name.toUpperCase() + '_LIB' ]
        cpp.includePaths: [ FileInfo.joinPaths(project.sourceDirectory, 'include', product.module) ]
        cpp.dynamicLibraries: [
            FileInfo.joinPaths(project.sourceDirectory, 'lib', product.module,
                               product.targetName) + product.cpp.variantSuffix
        ]
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
        files: 'resource/windows/library.rc'
    }
}
