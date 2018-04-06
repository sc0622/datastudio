import qbs
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile

PackProduct {

    Depends { name: 'cpp' }
    Depends { name: 'meta'; enabledInstall: false }

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
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
    }

    // config
    Group {
        name: 'data-config'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'config') + '/'
        files: [ '**/*.*' ]
        excludeFiles: [ '**/*.pr[io]', '**/*.qbs' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'config')
        qbs.installSourceBase: prefix
    }

    // app
    Group {
        name: 'data-app'
        prefix: FileInfo.joinPaths(projectDir, 'bin') + '/'
        files: [ project.projectName + project.variantSuffix + '.exe' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'bin')
    }

    // tools
    Group {
        name: 'data-tools'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'tools') + '/'
        files: [ 'WinSnap/*', 'DataAnalyse.exe' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'tools')
        qbs.installSourceBase: prefix
    }

    // msvc-runtime
    Group {
        name: 'data-runtime'
        prefix: FileInfo.joinPaths(Environment.getEnv('VCINSTALLDIR'), 'redist',
                                   (qbs.buildVariant == 'debug' ? 'debug_*' : ''),
                                   qbs.architecture, 'Microsoft.VC*.*CRT') + '/'
        files: [ 'concrt*.dll', 'msvcp*.dll', 'vccorlib*.dll', 'vcruntime*.dll' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'bin')
    }

    Rule {
        inputs: [ 'meta.in' ]
        Artifact{
            filePath: FileInfo.joinPaths(product.meta.targetPath, FileInfo.relativePath(
                                             product.meta.sourcePath, input.filePath))
            fileTags: [ 'meta.out' ]
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
                    //
                    target.write(content);
                    target.close();
                }
            }
            return [ cmd ]
        }
    }
}
