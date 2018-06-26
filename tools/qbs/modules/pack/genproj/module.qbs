import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile

Module {
    additionalProductTypes: [ 'setup.genproj' ]

    Depends { name: 'cpp' }
    Depends { name: 'Qt.core'; cpp.link: false }

    Group {
        name: 'files'
        files: [ 'setup.qbs.in' ]
        fileTags: [ 'setup.genproj.in' ]
    }

    Rule {
        inputs: [ 'setup.genproj.in' ]
        Artifact {
            fileTags: [ 'setup.genproj' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory,
                                         project.projectName + '-' + input.completeBaseName)
        }
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
            cmd.description = 'generating ' + output.fileName + '...';
            cmd.sourceCode = function() {
                var source = new TextFile(input.filePath, TextFile.ReadOnly);
                var content = source.readAll();
                source.close();
                // replace @QBS_MIN_VER@
                content = content.replace(/@QBS_MIN_VER@/g, project.minimumQbsVersion);
                // replace @VERSION@
                content = content.replace(/@VERSION@/g, project.version);
                // replace @VARIANT_SUFFIX@
                content = content.replace(/@VARIANT_SUFFIX@/g, project.variantSuffix);
                // replace @PROJECT_INSTALL_ROOT@
                var relativeInstallRoot = '../' + FileInfo.relativePath(project.sourceDirectory + '/..',
                                                                        project.installRoot);
                content = content.replace(/@PROJECT_INSTALL_ROOT@/g, relativeInstallRoot);
                // replace @PROJECT_NAME@
                content = content.replace(/@PROJECT_NAME@/g, project.projectName);
                // replace @PROJECT_DISPLAY_NAME@
                content = content.replace(/@PROJECT_DISPLAY_NAME@/g, project.projectDisplayName);
                //
                var target = new TextFile(output.filePath, TextFile.WriteOnly);
                target.write(content);
                target.close();
            }
            cmds.push(cmd);
            // remove project setup dir
            cmd = new JavaScriptCommand;
            cmd.setupBuildDir = project.sourceDirectory + '/../build-'
                    + project.projectName + '-setup-Qt_'
                    + product.Qt.core.version.replace(/\./g, '_')
                    + '-' + (product.qbs.buildVariant == 'debug'
                             ? 'Debug' : 'Release');
            if (FileInfo.cleanPath) {
                cmd.setupBuildDir = FileInfo.cleanPath(cmd.setupBuildDir);
            }
            cmd.description = 'removing setup build dir...';
            cmd.sourceCode = function() {
                File.remove(output.filePath + '.user');
                try { File.remove(setupBuildDir); } catch(msg) { console.warn(msg); }
            }
            cmds.push(cmd);
            return cmds;
        }
    }
}
