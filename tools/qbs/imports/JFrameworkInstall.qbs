import qbs
import qbs.File
import qbs.FileInfo
import '../../setenv/setenv.js' as Env

Product {
    name: 'JFrameworkInstall'
    type: [ 'header.out', 'library.out' ]

    Depends { name: 'cpp' }

    readonly property path jframeDir: Env.jframeDir(project)
    readonly property bool jframeExists: !!jframeDir
    property stringList modules3rdpart: []

    // headers

    Group {
        id: _3rdpart_header
        condition: jframeExists
        name: '3rdpart_header'
        prefix: jframeDir + '/include/3rdpart/'
        files: {
            var files = [];
            modules3rdpart.forEach(function(item){
                files.push(item + '/**/*.h');
                files.push(item + '/**/*.hh');
                files.push(item + '/**/*.hpp');
            });
            return files;
        }
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists
        inputs: _3rdpart_header.fileTags
        Artifact {
            fileTags: [ 'header.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
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
        condition: jframeExists
        name: '3rdpart_dynamic'
        prefix: jframeDir + '/lib/3rdpart/'
        files: {
            var files = [];
            modules3rdpart.forEach(function(item){
                files.push(item + Env.dylibSuffix(product));
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
        condition: jframeExists
        name: '3rdpart_library'
        prefix: _3rdpart_dynamic.prefix
        excludeFiles: [ Env.incDylibSuffixFuzzy(product) ]
        files: {
            var files = [];
            modules3rdpart.forEach(function(item){
                files.push(item + '*.lib');
                files.push(item + '*.dll');
            });
            return files;
        }
        fileTags: [ name + '.in' ]
    }

    Rule {
        condition: jframeExists
        inputs: _3rdpart_dynamic.fileTags.concat(_3rdpart_library.fileTags)
        Artifact {
            fileTags: [ 'library.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         FileInfo.relativePath(product.jframeDir, input.filePath))
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
