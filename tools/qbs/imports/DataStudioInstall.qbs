import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import tools.EnvUtils

Product {
    name: 'DataStudioInstall'
    type: [ 'header.out', 'library.out' ]

    Depends { name: 'cpp' }

    readonly property path datastudioDir: EnvUtils.datastudioDir(project)
    readonly property bool datastudioExists: !!datastudioDir
    readonly property bool thisProject: datastudioDir == project.sourceDirectory
    property stringList module3rdpart: []
    property stringList moduleCore: []
    property bool installMeta: false
    property bool installMoxa: false
    property bool installRass: false

    // 3rdpart - headers

    Group {
        id: _3rdpart_headers
        name: '3rdpart_headers'
        condition: {
            return datastudioExists && !thisProject
        }
        prefix: datastudioDir + '/include/3rdpart/'
        files: {
            var files = [];
            module3rdpart.forEach(function(item){
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
        condition: datastudioExists && !thisProject
        inputs: _3rdpart_headers.fileTags
        Artifact {
            fileTags: [ 'header.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
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
        condition: datastudioExists && !thisProject
        prefix: datastudioDir + '/lib/3rdpart/'
        files: {
            var files = [];
            module3rdpart.forEach(function(item){
                files.push(item + EnvUtils.dylibSuffix(qbs) + '*');
            })
            return files;
        }
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }

    Group {
        id: _3rdpart_library
        name: '3rdpart_library'
        condition: datastudioExists && !thisProject
        prefix: _3rdpart_dynamic.prefix
        files: {
            var files = [];
            module3rdpart.forEach(function(item){
                files.push(item + '*.lib');
                files.push(item + '*.dll');
            })
            return files;
        }
        excludeFiles: [ EnvUtils.incDylibFuzzy(qbs) ]
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: datastudioExists && !thisProject
        inputs: _3rdpart_dynamic.fileTags.concat(_3rdpart_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
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
        condition: datastudioExists && !thisProject
        prefix: datastudioDir + '/include/core/'
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
        condition: datastudioExists && !thisProject
        inputs: core_headers.fileTags
        Artifact {
            fileTags: [ 'header.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
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
        condition: datastudioExists && !thisProject
        prefix: datastudioDir + '/lib/core/'
        files: {
            var files = [];
            moduleCore.forEach(function(item){
                files.push(item + EnvUtils.dylibSuffix(qbs) + '*');
            })
            return files;
        }
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }

    Group {
        id: core_library
        name: 'core_library'
        condition: datastudioExists && !thisProject
        prefix: core_dynamic.prefix
        files: {
            var files = [];
            moduleCore.forEach(function(item){
                files.push(item + '*.lib');
                files.push(item + '*.dll');
            })
            return files;
        }
        excludeFiles: [ EnvUtils.incDylibFuzzy(qbs) ]
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: datastudioExists && !thisProject
        inputs: core_dynamic.fileTags.concat(core_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    //  meta

    Group {
        id: meta_dynamic
        name: 'meta_dynamic'
        condition: datastudioExists && installMeta && !thisProject
        prefix: datastudioDir + '/lib/core/plugins/meta/'
        files: [ '**/*.dll', '**/qmldir', '**/*.qml', '**/*.js', '**/imports/**/*' ]
        excludeFiles: [ EnvUtils.excDylibFuzzy(qbs, true) ]
        fileTags: [ name + '.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin/meta'
        qbs.installSourceBase: prefix
    }

    Group {
        id: meta_library
        condition: datastudioExists && installMeta && !thisProject
        name: 'meta_library'
        prefix: datastudioDir + '/lib/core/plugins/meta/'
        files: [ '**/*.dll', '**/*.qmltypes' ]
        excludeFiles: [ EnvUtils.incDylibFuzzy(qbs, true) ]
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: datastudioExists && installMeta && !thisProject
        inputs: meta_dynamic.fileTags.concat(meta_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }

    // moxa

    Group {
        id: dynamic_moxa
        name: 'dynamic_moxa'
        condition: installMoxa
        prefix: datastudioDir + '/lib/3rdpart/moxa/'
        files: [ 'pcomm' + (qbs.architecture == 'x86_64' ? '_x86_64' : '') + '.dll' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: datastudioExists && installMoxa && !thisProject
        inputs: dynamic_moxa.fileTags
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    // rass

    Group {
        id: dynamic_rass
        name: 'dynamic_rass'
        condition: installRass
        prefix: datastudioDir + '/lib/3rdpart/rass/'
        files: [ 'rass' + (qbs.buildVariant == 'debug' ? 'd' : '') + '_md.dll' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: datastudioExists && installRass && !thisProject
        inputs: dynamic_rass.fileTags
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.datastudioDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
