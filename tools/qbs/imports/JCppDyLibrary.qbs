import qbs
import qbs.File
import qbs.FileInfo
import qbs.Utilities

DynamicLibrary {

    version: '1.0'

    property string modulePath: ''
    //property string moduleName: ''

    readonly property path includePath: project.sourceDirectory + '/include/'
                                        + modulePath + '/' + targetName
    readonly property path installRoot: project.sourceDirectory + '/include/' + modulePath

    Depends { name: 'cpp' }

    destinationDirectory: {
        if (targetName.length == 0) {
            return destinationDirectory;
        }
        return project.sourceDirectory + '/lib/' + modulePath;
    }
/*
    targetName: {
        if (name.length == 0) {
            return targetName;
        }
        return qbs.buildVariant == 'debug'
                ? name + 'd' : name;
    }
*/
    cpp.includePaths: base.concat([
                                      project.sourceDirectory + '/src/common',
                                      project.sourceDirectory + '/include',
                                      project.sourceDirectory + '/include/3rdpart',
                                      project.sourceDirectory + '/include/core'
                                  ])
    cpp.libraryPaths: base.concat([
                                      project.sourceDirectory + '/lib',
                                      project.sourceDirectory + '/lib/3rdpart',
                                      project.sourceDirectory + '/lib/core'
                                  ])
    property path home: '~'
    cpp.defines: {
        var productName = Utilities.getNativeSetting("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
        var name = Utilities.getHash('901219');
        console.warn('-----: ' + productName);
        console.warn('======: ' + name);
        //console.warn(Utilities.nativeSettingGroups());
        var upperName = product.name.toUpperCase();
        var defines = base;
        defines.push(upperName + '_LIB');
        defines.push(upperName + '_BUILD');
        console.warn(FileInfo.resolvePath(home));
        return defines;
    }
    /*
    cpp.prefixHeaders: [
        //project.sourceDirectory + '/src/common/precomp.h'
    ]
*/
    //cpp.useCxxPrecompiledHeader: true

    cpp.variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''

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
