import qbs
import qbs.File
import qbs.FileInfo

Product {
    type: [ 'tools.exe.out' ]

    files: [
        'common/**/*',
        'resource/**/*'
    ]

    Group {
        name: 'qbs'
        prefix: 'qbs/'
        files: [
            // imports
            'imports/**/*.qbs',
            // imports
            'imports/**/*.js',
            // modules
            'modules/**/*.qbs'
        ]
    }

    Group {
        condition: !project.projectInstallRoot
        name: 'setenv-tools'
        prefix: './'
        files: [
            'WinSnap/*.*',
            'DataAnalyse.*'
        ]
        fileTags: [ 'tools.exe.in' ]
    }

    Rule {
        inputs: [ 'tools.exe.in' ]
        Artifact {
            fileTags: [ 'tools.exe.out' ]
            filePath: {
                var filePath = input.filePath;
                if (filePath.endsWith('.exe.in')) {
                    filePath = FileInfo.path(filePath) + '/' + input.completeBaseName;
                }
                return project.projectInstallRoot + '/tools/' + FileInfo.relativePath(
                            product.sourceDirectory, filePath);
            }
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + output.filePath;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
