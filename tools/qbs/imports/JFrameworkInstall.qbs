import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import tools.EnvUtils

Product {
    name: 'JFrameWorkInstall'
    type: [ 'header.out', 'library.out' ]

    Depends { name: 'cpp' }

    readonly property path jframeDir: EnvUtils.jframeDir(project)
    readonly property bool jframeExists: !!jframeDir
    readonly property bool thisProject: jframeDir == project.sourceDirectory
    property stringList module3rdpart: []
    property stringList moduleCore: []
    property stringList moduleCoreExt: []
    property stringList moduleComponent: []
    property bool installCore: false
    property bool installComponent: false
    property bool installJFrameworkdir: false
    property string thirdInstallDir: 'bin'
    property string coreInstallDir: 'bin'

    // 3rdpart - headers

    Group {
        id: _3rdpart_headers
        name: '3rdpart_headers'
        condition: jframeExists && !thisProject
        prefix: jframeDir + '/include/3rdpart/'
        files: {
            var files = [];
            module3rdpart.forEach(function(item){
                files.push(item + '/**/*');
            })
            return files;
        }
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists && !thisProject
        inputs: _3rdpart_headers.fileTags
        Artifact {
            fileTags: [ 'header.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    // 3rdpart - library

    Group {
        id: _3rdpart_dynamic
        name: '3rdpart_dynamic'
        condition: jframeExists
        prefix: jframeDir + '/lib/3rdpart/'
        files: {
            var files = [];
            module3rdpart.forEach(function(item){
                files.push(EnvUtils.libPrefix(qbs) + item + EnvUtils.dylibSuffix(qbs) + '*');
            })
            return files;
        }
        excludeFiles: [ '*.so' ]
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: thirdInstallDir
    }

    Group {
        id: _3rdpart_library
        name: '3rdpart_library'
        condition: jframeExists && !thisProject
        prefix: _3rdpart_dynamic.prefix
        files: {
            var files = [];
            if (qbs.targetOS.contains('windows')) {
                module3rdpart.forEach(function(item){
                    files.push(item + '*.lib');
                    files.push(item + '*.dll');
                })
            } else {
                module3rdpart.forEach(function(item){
                    files.push(EnvUtils.libPrefix(qbs) + item + '*' + EnvUtils.dylibExtension(qbs));
                })
            }
            return files;
        }
        excludeFiles: [ EnvUtils.incDylibFuzzy(qbs) ]
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists && !thisProject
        inputs: _3rdpart_dynamic.fileTags.concat(_3rdpart_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    //=================================================================

    // core - headers

    Group {
        id: core_headers
        name: 'core_headers'
        condition: jframeExists && installCore && !thisProject
        prefix: jframeDir + '/include/core/'
        files: {
            var files = [];
            moduleCore.forEach(function(item){
                files.push(item + '/**/*.h');
                files.push(item + '/**/*.hh');
                files.push(item + '/**/*.hpp');
                files.push(item + '/**/*.hxx');
            })
            return files;
        }
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists && installCore && !thisProject
        inputs: core_headers.fileTags
        Artifact {
            fileTags: [ 'header.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    // library

    Group {
        id: core_dynamic
        name: 'core_dynamic'
        condition: jframeExists && installCore && !thisProject
        prefix: jframeDir + '/lib/core/'
        files: {
            var files = [];
            moduleCore.forEach(function(item){
                files.push(EnvUtils.libPrefix(qbs) + item + EnvUtils.dylibSuffix(qbs) + '*');
            })
            return files;
        }
        excludeFiles: [ '*.so' ]
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: coreInstallDir
    }

    Group {
        id: core_library
        name: 'core_library'
        condition: jframeExists && installCore && !thisProject
        prefix: core_dynamic.prefix
        files: {
            var files = [];
            if (qbs.targetOS.contains('windows')) {
                moduleCore.forEach(function(item){
                    files.push(item + '*.lib');
                    files.push(item + '*.dll');
                })
            } else {
                moduleCore.forEach(function(item){
                    files.push(EnvUtils.libPrefix(qbs) + item + '*' + EnvUtils.dylibExtension(qbs));
                })
            }
            return files;
        }
        excludeFiles: [ EnvUtils.incDylibFuzzy(qbs) ]
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists && installCore && !thisProject
        inputs: core_dynamic.fileTags.concat(core_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    // core - dynamic - jframeworkenv、jframeworkdir

    Group {
        id: core_dynamic_ext
        name: 'core_dynamic_ext'
        condition: jframeExists && installCore && !thisProject
        prefix: jframeDir + '/lib/core/'
        files: {
            var files = [];
            moduleCoreExt.forEach(function(item){
                files.push(item + '.dll*');
            });
            return files;
        }
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin/core'
    }

    Rule {
        condition: jframeExists && installCore && !thisProject
        inputs: core_dynamic_ext.fileTags
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }

    Group {
        id: core_dynamic_jframeworkdir
        name: 'core_dynamic_jframeworkdir'
        condition: jframeExists && installJFrameworkdir && !thisProject
        prefix: jframeDir + '/lib/core/'
        files: [ 'jframeworkdir.dll*' ]
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }

    Rule {
        condition: jframeExists && installJFrameworkdir && !thisProject
        inputs: core_dynamic_jframeworkdir.fileTags
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }

    // component - jframe - dynamic

    Group {
        id: component_jframe_dynamic
        condition: jframeExists && installComponent && !thisProject
        name: 'component_jframe_dynamic'
        prefix: jframeDir + '/component/jframe/'
        files: {
            var files = [];
            moduleComponent.forEach(function(item){
                var prefix = item + '/' + item;
                files.push(prefix + '*.xml');
                files.push(prefix + EnvUtils.dylibSuffix(qbs) + '*');
            });
            return files;
        }
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'component/jframe'
        qbs.installSourceBase: prefix
    }

    // component - jframe - library

    Group {
        id: component_jframe_library
        condition: jframeExists && installComponent && !thisProject
        name: 'component_jframe_library'
        prefix: jframeDir + '/component/jframe/'
        files: {
            var files = [];
            moduleComponent.forEach(function(item){
                files.push(item + '/' + EnvUtils.dylibSuffix(qbs) + '*');
            });
            return files;
        }
        excludeFiles: [ EnvUtils.incDylibFuzzy(qbs) ]
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists
        inputs: component_jframe_dynamic.fileTags.concat(component_jframe_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }
}
