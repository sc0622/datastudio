import qbs
import qbs.File

Product {
    condition: false
    type: [ 'setup-clean' ]

    Rule {
        multiplex: true
        Artifact { fileTags: [ 'setup-clean' ] }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'removing setup dir...';
            cmd.sourceCode = function(){ File.remove(project.completeSetupDir); }
            return [ cmd ];
        }
    }
}
