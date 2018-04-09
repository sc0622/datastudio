import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile

Product {
    name: 'config'
    type: [ 'config.out' ]

    Depends { name: 'cpp' }

    Group {
        name: 'config'
        files: [ '**/*' ]
        excludeFiles: [ 'config.xml' ]
        fileTags: [ 'config.in' ]
        qbs.install: true
        qbs.installPrefix: project.setupDir
        qbs.installDir: name
        qbs.installSourceBase: '.'
    }

    Rule {
        inputs: [ 'config.in' ]
        multiplex: true
        Artifact { fileTags: [ 'config.out' ] }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = 'replacing config.xml...';
            cmd.sourceCode = function() {
                if (!project.version) {
                    console.warn('\'project.version\' is not exists!');
                    return;
                }
                var sourcePath = product.sourceDirectory + '/config.xml';
                var targetPath = project.completeSetupDir + '/config/config.xml';
                var source = new TextFile(sourcePath, TextFile.ReadOnly);
                var target = new TextFile(targetPath, TextFile.WriteOnly);
                var content = source.readAll().replace(/@VERSION@/g, project.version);
                source.close();
                target.write(content);
                target.close();
            }
            return [cmd];
        }
    }
}
