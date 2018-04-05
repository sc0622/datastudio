import qbs
import qbs.FileInfo

Product {
    type: [ 'setup-clean' ]
    Rule {
        condition: false
        multiplex: true
        Artifact { fileTags: [ 'setup-clean' ] }
        prepare: {
            var cmd = new Command('rd', [ '/s', '/q',
                                         FileInfo.toWindowsSeparators(project.completeSetupDir + '/config') ]);
            cmd.description = 'clean setup folder...';
            return [ cmd ];
        }
    }
}
