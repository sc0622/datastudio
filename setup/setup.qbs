import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

Project {

    readonly property string setupDir: 'setup'
    readonly property path completeSetupDir: FileInfo.joinPaths(qbs.installRoot, setupDir)
    readonly property path packagesPath: path + '/modules/packages';

    references: [
        'config/config.qbs',
        'packages/packages.qbs'
    ]

    Product {
        name: 'pack-build'
        type: 'pack.out'

        readonly property string ifwDir: FileInfo.fromWindowsSeparators(Environment.getEnv('QTIFW_DIR'))

        Depends { name: 'cpp' }
        Depends { name: 'config'; cpp.link: false }
        Depends { name: 'com.smartsoft.datastudio.3rdpart'; cpp.link: false }
        Depends { name: 'com.smartsoft.datastudio.core'; cpp.link: false }
        Depends { name: 'com.smartsoft.datastudio.app'; cpp.link: false }

        Rule {
            condition: File.exists(ifwDir)
            multiplex: true
            Artifact { fileTags: [ 'pack.out' ] }
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
                args.push('--config');
                args.push(FileInfo.joinPaths(project.completeSetupDir, 'config', 'config.xml'));
                // packages
                args.push('--packages');
                args.push(FileInfo.joinPaths(project.completeSetupDir, 'packages'));
                args.push(FileInfo.joinPaths(project.completeSetupDir, 'bin',
                                             project.projectName + '-v' + project.version));
                // resources
                var configQrc = FileInfo.joinPaths(project.completeSetupDir, 'config', 'config.qrc');
                if (File.exists(configQrc)) {
                    args.push('--resources');
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
