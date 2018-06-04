import qbs
import qbs.File
import qbs.FileInfo
import tools.EnvUtils

CppApplication {
    version: '1.0.0'

    // module
    readonly property path precompPath: path + '/common'

    Depends { name: 'Qt.core'; cpp.link: false }
    Depends { name: 'Qt.qminimal'; condition: Qt.core.staticBuild }
    Depends { name: 'desc'; required: false }
    Depends { name: 'setenv'; required: false; cpp.link: false }

    targetName: name + (qbs.buildVariant == 'debug' ? 'd' : '')
    destinationDirectory: {
        if (project.workDirectory) {
            return FileInfo.joinPaths(project.workDirectory, 'bin');
        } else if (project.projectName) {
            return FileInfo.joinPaths(qbs.inatallRoot, project.projectName, 'bin');
        } else {
            return qbs.inatallRoot;
        }
    }

    desc.condition: true
    desc.fileDesc: 'Smartsoft® Application'
    desc.version: version
    desc.productName: name

    cpp.defines: {
        var defines = [ 'PROJECT_DIR="' + project.sourceDirectory + '"' ];
        if (qbs.buildVariant == 'debug') {
            defines.push('DEBUG');
            defines.push('_DEBUG');
        }
        return defines;
    }
    cpp.includePaths: base.concat([precompPath])
    //cpp.variantSuffix: project.variantSuffix

    Properties {
        condition: qbs.targetOS.contains('linux')
        cpp.dynamicLibraries: [ 'dl' ]
    }

    Group {
        name: 'Precomp'
        condition: cpp.useCxxPrecompiledHeader
        prefix: precompPath + '/'
        files: ['precomp.h']
        fileTags: ['cpp_pch_src']
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
        files: 'resource/windows/application.rc'
    }

    Rule {
        inputs: ['j.scxml.compilable']
        Artifact {
            filePath: FileInfo.joinPaths(product.moduleProperty('Qt.core', 'generatedHeadersDir'),
                                         input.baseName + '.h')
            fileTags: [ 'hpp', 'unmocable' ]
        }
        Artifact {
            filePath: input.baseName + '.cpp'
            fileTags: [ 'cpp' ]
        }
        prepare: {
            var compilerName = product.moduleProperty('Qt.scxml', 'qscxmlcName');
            var compilerPath = FileInfo.joinPaths(input.moduleProperty('Qt.core', 'binPath'),
                                                  compilerName);
            var args = ['--header', outputs['hpp'][0].filePath,
                        '--impl', outputs['cpp'][0].filePath];
            var cxx98 = input.moduleProperty('cpp', 'cxxLanguageVersion') === 'c++98';
            if (cxx98)
                args.push('-no-c++11');
            var className = input.moduleProperty('Qt.scxml', 'className');
            if (className)
                args.push('--classname', className);
            var namespace = input.moduleProperty('Qt.scxml', 'namespace');
            if (namespace)
                args.push('--namespace', namespace);
            args.push('--statemethods');
            args.push(input.filePath);
            // create command compilier
            var cmd = new Command(compilerPath, args);
            cmd.description = 'compiling ' + input.fileName;
            cmd.highlight = 'codegen';
            return [ cmd ];
        }
    }

    //
    /*
    readonly property string dep3rdPrefix: probeEnv.dep3rdPrefix
    readonly property string depCorePrefix: probeEnv.depCorePrefix

    Probe {
        id: probeEnv
        property string dep3rdPrefix: '3rdpart.'
        property string depCorePrefix: 'core.'
        readonly property var proj: project
        configure: {
            dep3rdPrefix = EnvUtils.jframe3rdpartPrefix(proj);
            depCorePrefix = EnvUtils.datastudioCorePrefix(proj);
        }
    }*/
}
