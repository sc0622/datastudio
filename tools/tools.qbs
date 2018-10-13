import qbs
import qbs.File
import qbs.FileInfo

Product {
    type: [ 'tools.exe.out' ]

    Group {
        name: 'qbs'
        prefix: 'qbs/'
        files: [ '**/*' ]
    }

    Group {
        condition: !project.projectInstallRoot && qbs.targetOS.contains('windows')
        name: 'setenv-tools'
        prefix: './'
        files: [
            'WinSnap/*.*',
            'DataAnalyse.*'
        ]
        fileTags: [ 'tools.exe.in' ]
    }

    Rule {
        condition: qbs.targetOS.contains('windows')
        inputs: [ 'tools.exe.in' ]
        Artifact {
            fileTags: [ 'tools.exe.out' ]
            filePath: {
                var filePath = input.filePath;
                if (filePath.endsWith('.exe.in')) {
                    filePath = FileInfo.path(filePath) + '/' + input.completeBaseName;
                }
                var filePath = project.workDirectory + '/tools/' + FileInfo.relativePath(
                            product.sourceDirectory, filePath);
                return filePath;
            }
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + output.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
