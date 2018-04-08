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

    Project {
        name: 'global'
        Product {
            id: setup_generatpr
            name: 'setup-generator'
            type: [ 'setup.generator.out' ]
            Depends { name: 'cpp'; cpp.link: false }
            Depends { name: 'Qt.core'; cpp.link: false }
            Group {
                id: group_config
                name: 'config'
                files: [
                    'CHANGE',
                    'LICENSE',
                    'README.md',
                    'VERSION',
                    'setup.qbs.in'
                ]
                fileTags: [ 'setup.generator.in' ]
                qbs.install: true
                qbs.installPrefix: project.projectName
            }

            Rule {
                inputs: group_config.fileTags
                multiplex: true
                Artifact { fileTags: setup_generatpr.type }
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
                    var targetName = project.projectName + '-setup.qbs';
                    cmd = new JavaScriptCommand;
                    cmd.targetName = targetName;
                    cmd.description = 'generating \'' + cmd.targetName + '\' file...';
                    cmd.minimumQbsVersion = project.minimumQbsVersion;
                    cmd.projectVersion = project.version
                    cmd.variantSuffix = project.variantSuffix
                    cmd.projectDirectory = project.sourceDirectory;
                    cmd.projectName = project.projectName;
                    cmd.projectInstallRoot = project.inatallRoot;
                    cmd.sourceCode = function() {
                        var targetPath = projectDirectory + '/' + targetName;
                        var sourcePath = projectDirectory + '/' + 'setup.qbs.in';
                        if (!File.exists(sourcePath)) {
                            console.warn('file \'' + sourcePath + '\' is not exists!');
                            return;
                        }
                        var source = new TextFile(sourcePath, TextFile.ReadOnly);
                        var content = source.readAll();
                        source.close();
                        // replace @QBS_MIN_VER@
                        content = content.replace(/@QBS_MIN_VER@/g, minimumQbsVersion);
                        // replace @VERSION@
                        content = content.replace(/@VERSION@/g, projectVersion);
                        // replace @VARIANT_SUFFIX@
                        content = content.replace(/@VARIANT_SUFFIX@/g, variantSuffix);
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
                    // remove project setup dir
                    cmd = new JavaScriptCommand;
                    cmd.setupBuildDir = FileInfo.cleanPath(project.sourceDirectory + '/../build-'
                                                           + project.projectName + '-setup-Qt_'
                                                           + product.Qt.core.version.replace(/\./g, '_')
                                                           + '-' + (product.qbs.buildVariant == 'debug'
                                                                    ? 'Debug' : 'Release'));
                    cmd.description = 'removing setup build dir...';
                    cmd.targetName = targetName;
                    cmd.sourceCode = function() {
                        File.remove(project.sourceDirectory + '/' + targetName + '.user');
                        try { File.remove(setupBuildDir); } catch(msg) { console.log(msg); }
                    }
                    cmds.push(cmd);
                    return cmds;
                }
            }
        }
    }

    Probe {
        id: probeVersion
        property string version: '0.0.0'
        readonly property path projectDirectory: project.sourceDirectory
        configure: {
            var filePath = FileInfo.joinPaths(projectDirectory, 'VERSION');
            var file = new TextFile(filePath, TextFile.ReadOnly);
            version = file.readAll().trim();
            file.close();
            found = true;
        }
    }
}
