import qbs
import qbs.File
import qbs.FileInfo

DynamicLibrary {

    version: '1.0'

    property string modulePath: ''
    property string moduleName: ''

    readonly property string includePath: project.sourceDirectory + '/include/'
                                          + modulePath + '/' + moduleName
    readonly property string installRoot: project.sourceDirectory + '/include/' + modulePath

    Depends { name: 'cpp' }

    destinationDirectory: {
        if (moduleName.length == 0) {
            return destinationDirectory;
        }
        return project.sourceDirectory + '/lib/' + modulePath;
    }

    targetName: {
        if (moduleName.length == 0) {
            return targetName;
        }
        return qbs.buildVariant == 'debug'
                ? moduleName + 'd' : moduleName;
    }

    cpp.compilerIncludePaths: [
        project.sourceDirectory + '/src/common',
        project.sourceDirectory + '/include',
        project.sourceDirectory + '/include/3rdpart',
        project.sourceDirectory + '/include/core'
    ]

    cpp.compilerLibraryPaths: [
        project.sourceDirectory + '/lib',
        project.sourceDirectory + '/lib/3rdpart',
        project.sourceDirectory + '/lib/core'
    ]

    cpp.prefixHeaders: [
        //project.sourceDirectory + '/src/common/precomp.h'
    ]

    cpp.useCxxPrecompiledHeader: true

    Group {
        name: "precomp"
        prefix: project.sourceDirectory + '/src/common/'
        files: ["precomp.h"]
        fileTags: ["cpp_pch_src"]
    }

    Rule {
        inputs: [ 'hpp.in' ]
        Artifact {
            filePath: product.includePath  + '/'
                      + FileInfo.relativePath(product.sourceDirectory, input.filePath)
            fileTags: ["hpp"]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "Coping " + input.fileName;// + ' (' + output.filePath + ')';
            //cmd.silent = true;
            cmd.sourceCode = function() {
                File.copy(input.filePath, output.filePath);
            }
            return [cmd];
        }
    }

    Export {
        Depends { name: 'cpp' }
        cpp.includePaths: sourceDirectory
    }
}
