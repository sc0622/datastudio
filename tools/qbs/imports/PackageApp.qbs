import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import qbs.Environment

PackageProduct {
    version: project.version

    property bool installGlobal: true
    property bool installSystem32Api: true
    property bool installMsvcRuntime: true
    property string projectName: project.projectName
    property string appName: projectName + project.variantSuffix
    property string displayName: project.projectDisplayName

    Group {
        name: 'data-global'
        condition: installGlobal
        prefix: project.sourceDirectory + '/'
        files: [
            'LICENSE',
            'README.md',
            'VERSION'
        ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
    }

    // system32 - api*
    Group {
        name: 'data-sys-api'
        condition: installSystem32Api && qbs.targetOS.contains('windows')
        prefix: FileInfo.joinPaths(Environment.getEnv('SystemRoot'), 'system32') + '/'
        files: [ 'api-ms-win-*.dll' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'bin'
    }

    // msvc-runtime
    Group {
        name: 'data-runtime'
        condition: installSystem32Api && qbs.targetOS.contains('windows')
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
                    content = content.replace(/@VERSION@/g, product.version);
                    // replace @PROJECT_NAME@
                    content = content.replace(/@PROJECT@/g, product.projectName);
                    // replace @APPNAME@
                    content = content.replace(/@APPNAME@/g, product.appName);
                    // replace @PROJECT_DISPLAY_NAME@
                    content = content.replace(/@PROJECT_DISPLAY_NAME@/g, product.displayName);
                    //
                    target.write(content);
                    target.close();
                }
            }
            return [ cmd ]
        }
    }
}
