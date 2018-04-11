import qbs
import qbs.File
import qbs.FileInfo
import '../../setenv.js' as Func

Module {
    id: root
    additionalProductTypes: [ 'header.out', 'library.out' ]
    Depends { name: 'cpp' }

    readonly property path jframeDir: Func.jframeDir(project)
    readonly property stringList depends3rdpart: [
        'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp', 'nodeeditor', 'qwt', 'tinyxml'
    ]

    // headers

    Group {
        id: _3rdpart_header
        name: '3rdpart_header'
        prefix: root.jframeDir + '/include/3rdpart/'
        files: {
            var files = [];
            root.depends3rdpart.forEach(function(item){
                files.push(item + '/**/*.h');
                files.push(item + '/**/*.hh');
                files.push(item + '/**/*.hpp');
            });
            return files;
        }
        fileTags: [ name + '.in' ]
    }

    Rule {
        inputs: _3rdpart_header.fileTags
        Artifact {
            fileTags: [ 'header.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(root.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }

    // library

    Group {
        id: _3rdpart_dynamic
        name: '3rdpart_dynamic'
        prefix: root.jframeDir + '/lib/3rdpart/'
        files: {
            var files = [];
            root.depends3rdpart.forEach(function(item){
                files.push(item + Func.dylibSuffix(product));
            });
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
        prefix: _3rdpart_dynamic.prefix
        excludeFiles: [ Func.incDylibSuffixFuzzy(product) ]
        files: {
            var files = [];
            root.depends3rdpart.forEach(function(item){
                files.push(item + '*.lib');
                files.push(item + '*.dll');
            });
            return files;
        }
        fileTags: [ name + '.in' ]
    }

    Rule {
        inputs: _3rdpart_dynamic.fileTags.concat(_3rdpart_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(root.jframeDir, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }
}
