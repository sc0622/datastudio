import qbs
import qbs.File
import qbs.FileInfo
import tools.EnvUtils

DynamicLibrary {
    version: '1.0.0'
    property bool useQt: true
    property bool useQtCore: true

    type: base.concat([ 'translation' ])

    Depends { name: 'cpp' }
    Depends { name: 'Qt.core'; cpp.link: useQt && useQtCore }
    Depends { name: 'desc'; cpp.link: false }
    Depends { name: 'setenv'; required: false; cpp.link: false }

    // module
    property string module: ''
    readonly property path includePath: project.sourceDirectory + '/include/'
                                        + module + '/' + name
    readonly property path precompPath: path + '/common'

    // translation
    property path langPath: sourceDirectory + '/resource/lang'
    property pathList noRecursivePaths: []
    property pathList recursivePaths: []
    property stringList translationFileTags: [ 'hpp', 'cpp' ]
    property stringList translations: []
    property bool defaultTranslation: false

    // default flags
    property string relativeDirectory: ''
    property bool defaultCopyHeader: true
    property bool defaultCopyDynamic: true

    // install
    property stringList installFileTags: [ 'dynamiclibrary', "dynamiclibrary_symlink" ]
    property bool defaultInstall: true
    property string installPrefix: 'bin'
    property string installDir: ''

    // export
    property bool defaultExport: true

    targetName: name
    desc.condition: true
    desc.fileDesc: 'Smartsoft® Runtime Library'
    desc.version: version
    desc.productName: name

    cpp.includePaths: base.concat([ precompPath ])
    cpp.defines: {
        var defines = base.concat([ 'PROJECT_DIR="' + project.sourceDirectory + '"' ])
        var upperName = product.name.toUpperCase();
        defines.push(upperName + '_LIB');
        defines.push(upperName + '_BUILD');
        return defines;
    }
    cpp.variantSuffix: project.variantSuffix

    // for apple
    bundle.isBundle: false
    cpp.separateDebugInformation: qbs.debugInformation

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
            var args = [];
            // no-recursive
            if (product.noRecursivePaths.length > 0) {
                args.push('-no-recursive');
                product.noRecursivePaths.forEach(function(item){
                    args.push(item);
                })
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
            // ts
            if (product.translations.length > 0) {
                args.push('-ts');
                product.translations.forEach(function(item){
                    var filePath = FileInfo.joinPaths(product.langPath, item);
                    var path = FileInfo.path(filePath);
                    if (!File.exists(path)) {
                        File.makePath(path);
                    }
                    args.push(filePath);
                });
            }
            // create command
            var cmd = new Command(product.Qt.core.binPath + '/lupdate', args);
            cmd.description = 'generating translation file...';
            return [ cmd ];
        }
    }

    Export {
        condition: product.defaultExport
        Depends { name: 'cpp' }
        cpp.defines: [ product.name.toUpperCase() + '_LIB' ]
        cpp.includePaths: [ FileInfo.joinPaths(project.sourceDirectory, 'include', product.module) ]
        Properties {
            condition: qbs.targetOS.contains('windows')
            cpp.dynamicLibraries: [
                FileInfo.joinPaths(project.sourceDirectory, 'lib', product.module,
                                   product.targetName + product.cpp.variantSuffix)
            ]
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
            var defines = outer.concat(['RC_ICON1="' + desc.iconName + '"',
                                        'RC_FILEDESC=' + desc.fileDesc.replace(/ /g, '\\x20'),
                                        'RC_PRODUCTNAME=' + desc.productName.replace(/ /g, '\\x20'),
                                        'RC_COMPANYNAME=' + desc.companyName.replace(/ /g, '\\x20'),
                                        'RC_VERSION=' + desc.version.replace(/\./g, ',') + ',0',
                                        'RC_VERSION_STRING=' + desc.displayVersion,
                                        'RC_COPYRIGHT=2016-' + desc.copyrightYear
                                        + ' ' + desc.companyName.replace(/ /g, '\\x20')])
            if (desc.iconName && File.exists(desc.iconName)) {
                defines.push('GENERATE_ICON');
            }
            return defines;
        }
        files: 'resource/windows/library.rc'
    }
}
