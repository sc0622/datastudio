import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile

PackageProduct {
    productPath: path
    type: base.concat([ 'data.tools.exe.out' ])

    // global
    Group {
        name: 'data-global'
        prefix: project.sourceDirectory + '/'
        files: [
            //'CHANGE',
            'LICENSE',
            'README.md',
            'VERSION'
        ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
    }

    // config
    Group {
        name: 'data-config'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'config') + '/'
        files: [ '**/*.*' ]
        excludeFiles: [ '**/*.pr[io]', '**/*.qbs', '**/~*' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'config'
        qbs.installSourceBase: prefix
    }

    // app
    Group {
        name: 'data-app'
        prefix: FileInfo.joinPaths(projectDir, 'bin') + '/'
        files: [ project.projectName + project.variantSuffix + '.exe' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'bin'
    }

    // tools
    Group {
        name: 'data-tools'
        prefix: project.sourceDirectory + '/tools/'
        files: [ 'WinSnap/*', 'DataAnalyse.exe.in' ]
        fileTags: [ 'data.tools.exe.in' ]
    }

    Rule {
        inputs: [ 'data.tools.exe.in' ]
        Artifact {
            fileTags: [ 'data.tools.exe.out' ]
            filePath: {
                var filePath = input.filePath;
                if (filePath.endsWith('.exe.in')) {
                    filePath = FileInfo.path(filePath) + '/' + input.completeBaseName;
                }
                return product.dataDir + '/tools/' + FileInfo.relativePath(
                            product.sourceDirectory, filePath);
            }
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    // msvc-runtime
    Group {
        name: 'data-runtime'
        prefix: FileInfo.joinPaths(Environment.getEnv('VCINSTALLDIR'), 'redist',
                                   (qbs.buildVariant == 'debug' ? 'debug_*' : ''),
                                   qbs.architecture, 'Microsoft.VC*.*CRT') + '/'
        files: [ 'concrt*.dll', 'msvcp*.dll', 'vccorlib*.dll', 'vcruntime*.dll' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'bin'
    }

    Rule {
        inputs: [ 'meta.in' ]
        Artifact {
            fileTags: [ 'meta.out' ]
            filePath: FileInfo.joinPaths(product.metaTargetPath, FileInfo.relativePath(
                                             product.metaSourcePath, input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.highlight = 'gencode';
            cmd.description = 'replacing content of \'' + input.fileName + '\' and moving to packages...';
            cmd.sourceCode = function() {
                var fileName = input.fileName;
                if (fileName.endsWith('.js') || fileName.endsWith('.xml')) {
                    var source = new TextFile(input.filePath, TextFile.ReadOnly);
                    var target = new TextFile(output.filePath, TextFile.WriteOnly);
                    var content = source.readAll();
                    source.close();
                    // replace @VERSION@
                    content = content.replace(/@VERSION@/g, project.version);
                    // replace @PROJECT_NAME@
                    content = content.replace(/@PROJECT@/g, project.projectName);
                    // replace @APPNAME@
                    content = content.replace(/@APPNAME@/g, project.projectName + project.variantSuffix);
                    // replace @APPNAME@
                    content = content.replace(/@PROJECT_DISPLAY_NAME@/g, project.projectDisplayName);
                    //
                    target.write(content);
                    target.close();
                }
            }
            return [ cmd ]
        }
    }
}
