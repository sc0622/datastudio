import qbs
import qbs.File
import qbs.FileInfo
import qbs.Process
import qbs.Environment
import qbs.TextFile

Project {

    PackProduct {
        name: 'pack-config'
        type: base.concat([ 'config.out' ])
        Group {
            name: 'config'
            prefix: 'config/'
            files: [ '**/*' ]
            excludeFiles: [ 'config.xml' ]
            qbs.install: true
            qbs.installPrefix: project.setupDir
            qbs.installDir: name
            qbs.installSourceBase: prefix
        }

        Group {
            name: 'config.xml'
            prefix: 'config/'
            files: [ 'config.xml' ]
            fileTags: [ 'config.in' ]
        }

        Rule {
            inputs: [ 'config.in' ]
            Artifact {
                filePath: FileInfo.joinPaths(project.completeSetupDir, 'config', input.fileName)
                fileTags: [ 'config.out' ]
            }
            prepare: {
                var cmd = new JavaScriptCommand();
                cmd.description = 'replacing ' + input.fileName;
                cmd.sourceCode = function() {
                    if (!project.version) {
                        console.warn('\'project.version\' is not exists!');
                        return;
                    }
                    var source = new TextFile(input.filePath, TextFile.ReadOnly);
                    var target = new TextFile(output.filePath, TextFile.WriteOnly);
                    var content = source.readAll().replace(/@VERSION@/g, project.version);
                    source.close();
                    target.write(content);
                    target.close();
                }
                return [cmd];
            }
        }
    }

    references: [
        'packages/packages.qbs'
    ]

    Product {
        name: 'pack-build'
        type: [ 'pack-build' ]

        readonly property string ifwDir: FileInfo.fromWindowsSeparators(Environment.getEnv('QTIFW_DIR'))

        Depends { name: 'cpp' }

        Rule {
            condition: File.exists(ifwDir)
            multiplex: true
            explicitlyDependsOn: [
                'pack-config',
                'packages',
                'com.smartsoft.datastudio.3rdpart',
                'com.smartsoft.datastudio.app',
                'com.smartsoft.datastudio.core'
            ]
            Artifact { fileTags: [ 'pack-build' ] }
            prepare: {
                var cmdMkBin = new JavaScriptCommand;
                cmdMkBin.description = 'making \'bin\' path';
                cmdMkBin.sourceCode = function() {
                    // make 'bin' path
                    File.makePath(FileInfo.joinPaths(project.completeSetupDir, 'bin'));
                }
                var args = [];
                // base arguments
                args.push('--verbose');
                args.push('--offline-only');
                // config.xml
                args.push('-c');
                args.push(FileInfo.joinPaths(project.completeSetupDir, 'config', 'config.xml'));
                // packages
                args.push('-p');
                args.push(FileInfo.joinPaths(project.completeSetupDir, 'packages'));
                args.push(FileInfo.joinPaths(project.completeSetupDir, 'bin',
                                             project.projectName + '-v' + project.version));
                // resources
                var configQrc = FileInfo.joinPaths(project.completeSetupDir, 'config', 'config.qrc');
                if (File.exists(configQrc)) {
                    args.push('-r');
                    args.push(configQrc);
                }
                // create command
                var cmdBuild = new Command(FileInfo.joinPaths(product.ifwDir, 'bin', 'binarycreator'), args);
                cmdBuild.highlight = 'linker';
                cmdBuild.description = 'generating installer file...';
                return [ cmdMkBin, cmdBuild ];
            }
        }
    }
}
