import qbs
import qbs.File
import qbs.FileInfo

ComLibrary {
    installDir: FileInfo.joinPaths('jframe', name)
    destinationDirectory: FileInfo.joinPaths(project.sourceDirectory,
                                             module, 'jframe', name)

    Rule {
        multiplex: true
        inputs: [ 'config.in' ]
        Artifact {
            filePath: FileInfo.joinPaths(
                          product.destinationDirectory, FileInfo.relativePath(
                              FileInfo.joinPaths(product.sourceDirectory, product.relativeDirectory),
                              input.filePath))
            fileTags: [ 'config' ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "coping " + input.fileName;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }
}
