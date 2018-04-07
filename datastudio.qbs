import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile

Project {
    minimumQbsVersion: '1.11.0'
    qbs.enableDebugCode: true
    qbsSearchPaths: [ 'tools/qbs' ]

    property string version: probeVersion.version
    readonly property int versionMajor: parseInt(version.split('.')[0])
    readonly property int versionMinor: parseInt(version.split('.')[1])
    readonly property int versionPatch: parseInt(version.split('.')[2])
    readonly property string projectName: 'datastudio'
    readonly property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''
    readonly property path qtdir: FileInfo.fromWindowsSeparators(Environment.getEnv('QTDIR'))
    readonly property path inatallRoot: qbs.installRoot

    references: [
        'tools/setenv/setenv.qbs',
        'config/config.qbs',
        'src/src.qbs',
        //'setup/setup.qbs',
        'tools/tools.qbs'
    ]

    Product {
        name: 'global'
        type: [ 'global.out' ]
        Depends { name: 'cpp'; cpp.link: false }

        Group {
            name: 'config'
            files: [
                'CHANGE',
                'LICENSE',
                'README.md',
                'VERSION',
                project.projectName + '-setup.qbs.in'
            ]
            fileTags: [ 'global.in' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
        }

        Rule {
            inputs: [ 'global.in' ]
            multiplex: true
            Artifact { fileTags: [ 'global.out' ] }
            prepare: {
                var cmds = [];
                // VERSION
                var cmd = new JavaScriptCommand;
                cmd.description = 'reading \'VERSION\' file...';
                cmd.sourceCode = function() {
                    var filePath = FileInfo.joinPaths(project.sourceDirectory, 'VERSION');
                    var file = new TextFile(filePath, TextFile.ReadOnly);
                    project.version = file.readAll().trim();
                    file.close();
                }
                cmds.push(cmd);
                // SETUP
                cmd = new JavaScriptCommand;
                cmd.targetName = project.projectName + '-setup.qbs';
                cmd.description = 'generating \'' + cmd.targetName + '\' file...';
                cmd.projectDirectory = project.sourceDirectory;
                cmd.projectName = project.projectName;
                cmd.projectInstallRoot = project.inatallRoot;
                cmd.sourceCode = function() {
                    var targetPath = projectDirectory + '/' + targetName;
                    var sourcePath = targetPath + '.in';
                    if (!File.exists(sourcePath)) {
                        console.warn('file \'' + sourcePath + '\' is not exists!');
                        return;
                    }
                    var source = new TextFile(sourcePath, TextFile.ReadOnly);
                    var content = source.readAll();
                    source.close();
                    // replace @PROJECT_INSTALL_ROOT@
                    var relativeInstallRoot = '../' + FileInfo.relativePath(projectDirectory + '/..',
                                                                            projectInstallRoot);
                    content = content.replace(/@PROJECT_INSTALL_ROOT@/g, relativeInstallRoot);
                    // replace @PROJECT_NAME@
                    content = content.replace(/@PROJECT_NAME@/g, projectName);
                    //
                    var target = new TextFile(targetPath, TextFile.WriteOnly);
                    target.write(content);
                    target.close();
                }
                cmds.push(cmd);
                return cmds;
            }
        }
    }

    Probe {
        id: probeVersion
        property string version: '0.0.0'
        readonly property path projectDirectory: sourceDirectory
        configure: {
            var filePath = FileInfo.joinPaths(projectDirectory, 'VERSION');
            var file = new TextFile(filePath, TextFile.ReadOnly);
            version = file.readAll().trim();
            file.close();
            found = true;
        }
    }
}
