import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile
import tools.EnvUtils

Module {
    additionalProductTypes: [ 'pack.out' ]

    property string productName: project.setupName ? project.setupName : project.projectName

    Depends { productTypes: [ 'setup.config', 'setup.package' ]; limitToSubProject: true }

    Rule {
        condition: File.exists(EnvUtils.ifwDir())
        multiplex: true
        Artifact {
            fileTags: [ 'pack.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory, 'setup', 'bin',
                                         product.pack.build.productName
                                         + '-' + project.version + '.exe')
        }
        prepare: {
            var cmdMkBin = new JavaScriptCommand;
            cmdMkBin.description = 'making \'bin\' path...';
            cmdMkBin.sourceCode = function() {
                File.makePath(FileInfo.path(output.fiilePath));
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
            args.push(output.filePath);
            // resources
            var configQrc = FileInfo.joinPaths(project.completeSetupDir, 'config', 'config.qrc');
            if (File.exists(configQrc)) {
                args.push('--resources');
                args.push(configQrc);
            }
            // create command
            var cmdBuild = new Command(FileInfo.joinPaths(EnvUtils.ifwDir(), 'bin', 'binarycreator'), args);
            cmdBuild.highlight = 'linker';
            cmdBuild.description = 'generating ' + output.fileName + '...';
            return [ cmdMkBin, cmdBuild ];
        }
    }

    validate: {
        var ifwDir = EnvUtils.ifwDir();
        if (!ifwDir) {
            throw '"ifwDir" is invalid!';
        }
        if (!File.exists(ifwDir)) {
            throw '"' + ifwDir + '" is not exists!';
        }
        //
        if (!product.pack.build.productName) {
            throw 'variable "product.pack.build.productName" is invalid!';
        }
    }
}
