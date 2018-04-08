import qbs

Module {

    additionalProductTypes: [ 'config.out' ]

    Group {
        name: 'config'
        prefix: 'config/'
        files: [ '**/*' ]
        excludeFiles: [ 'config.xml' ]
        fileTags: [ 'pack.in' ]
        qbs.install: true
        qbs.installPrefix: project.setupDir
        qbs.installDir: name
        qbs.installSourceBase: prefix
    }

    Group {
        name: 'config_xml'
        files: [ 'config.xml' ]
        fileTags: [ 'config.in' ]
    }

    Rule {
        inputs: [ 'config.in' ]
        Artifact {
            filePath: FileInfo.joinPaths(project.completeSetupDir, 'config', input.fileName)
            fileTags: [ 'pack.in', 'config.out' ]
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
