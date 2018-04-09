import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.Process
import qbs.Utilities

PluginLibrary {

    property pathList dynamicLibraryPaths: [
        project.sourceDirectory + '/lib/3rdpart',
        project.sourceDirectory + '/lib/core'
    ]

    Depends { name: 'Qt.qml' }

    Group {
        name: 'qmlfiles'
        prefix: product.sourceDirectory + '/'
        files: [ 'qmldir' ]
        fileTags: [ 'qmldir.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: product.installDir
    }

    type: base.concat([ 'qmldir.out', 'qmltype.out' ])

    Rule {
        inputs: [ 'qmldir.in' ]
        Artifact {
            filePath: FileInfo.joinPaths(product.destinationDirectory, input.fileName)
            fileTags: [ 'qmldir.out' ]
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function(){ File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }

    Rule {
        multiplex: true
        inputs: [ 'dynamiclibrary', 'qmldir.in' ]
        Artifact { fileTags: [ 'qmltype.out' ] }
        prepare: {
            var exeSuffix = (product.qbs.targetOS.contains('windows') ? '.exe' : '');
            var dylibsuffix = (product.qbs.targetOS.contains('windows') ? '.dll' : '');;
            var targetName = 'qmltypes';
            var targetPath = FileInfo.joinPaths(product.buildDirectory, targetName);
            var cmds = [], cmd;
            //
            if (!project.qtdir || !File.exists(ptoject.qtdir)) {
                console.warn('QTDIR is invalid!');
                return [];
            }
            var envs = ['PATH='
                        + Environment.getEnv('PATH') + ';'
                        + project.qtdir + '/bin;'
                        + product.dynamicLibraryPaths.join(';')];
            // sync global depends
            cmd = new JavaScriptCommand;
            cmd.description = 'sync global depends ...';
            cmd.qtdir = qtdir;
            cmd.exeSuffix = exeSuffix;
            cmd.dylibsuffix = dylibsuffix;
            cmd.targetPath = targetPath;
            cmd.sourceCode = function(){
                File.makePath(targetPath);
                // qmlplugindump
                filName = 'qmlplugindump' + exeSuffix;
                File.copy(FileInfo.joinPaths(project.qtdir, 'bin', filName), targetPath + '/' + filName);
                // platforms - qminimal
                var target = FileInfo.joinPaths(targetPath, 'platforms');
                File.makePath(target);
                filName = 'qminimal' + dylibsuffix;
                File.copy(FileInfo.joinPaths(project.qtdir, 'plugins','platforms', filName),
                          target + '/' + filName);
            }
            cmds.push(cmd);
            // windeployqt
            cmd = new Command(FileInfo.joinPaths(project.qtdir, 'bin', 'windeployqt'),
                              [targetPath, '--no-translations', '--' + product.qbs.buildVariant,
                               '--verbose', '0'/*, '--list', 'relative'*/]);
            cmd.description = 'execute windeployqt...';
            cmd.workingDirectory = targetPath;
            cmd.environment = envs;
            cmds.push(cmd);
            // generate
            cmd = new Command(FileInfo.joinPaths(targetPath, 'qmlplugindump'),
                              ['-nonrelocatable', product.uri, product.pluginVersion,
                               product.destinationRoot, '-output', FileInfo.joinPaths(
                                   product.destinationDirectory, product.targetName + '.qmltypes')]);
            cmd.description = 'generating qmltypes...';
            cmd.workingDirectory = targetPath;
            cmd.environment = envs;
            cmds.push(cmd)
            // remove qmltypes
            if (product.qbs.targetOS.contains('windows')) {
                cmd = new JavaScriptCommand;
                cmd.description = 'removing qmltypes dir...';
                cmd.targetPath = targetPath;
                cmd.sourceCode = function(){ try { File.remove(targetPath); } catch(msg) {} }
                cmds.push(cmd);
            }
            return cmds;
        }
    }
}
