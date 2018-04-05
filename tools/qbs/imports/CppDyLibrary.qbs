import qbs
import qbs.File
import qbs.FileInfo

DynamicLibrary {
    version: '1.0'

    property string module: ''
    readonly property path includePath: project.sourceDirectory + '/include/'
                                        + module + '/' + name
    readonly property path precompPath: project.sourceDirectory + '/src/common'
    property string relativeDirectory: ''
    property bool defaultCopyHeader: true
    property bool defaultCopyDynamic: true
    property stringList installFileTags: [ 'dynamiclibrary' ]
    property bool defaultExport: true
    property bool defaultInstall: true
    property string installPrefix: 'bin'
    property string installDir: module

    Depends { name: 'cpp' }

    targetName: name

    cpp.includePaths: base.concat([
                                      precompPath,
                                      project.sourceDirectory + '/include',
                                      project.sourceDirectory + '/include/3rdpart',
                                      project.sourceDirectory + '/include/core'
                                  ])
    cpp.libraryPaths: base.concat([
                                      project.sourceDirectory + '/lib',
                                      project.sourceDirectory + '/lib/3rdpart',
                                      project.sourceDirectory + '/lib/core'
                                  ])
    cpp.defines: {
        var upperName = product.name.toUpperCase();
        var defines = base;
        defines.push(upperName + '_LIB');
        defines.push(upperName + '_BUILD');
        return defines;
    }
    cpp.variantSuffix: project.variantSuffix

    Group {
        fileTagsFilter: installFileTags
        qbs.install: defaultInstall
        qbs.installPrefix: project.projectName
        qbs.installDir: installPrefix
    }

    Group {
        name: 'Precomp'
        condition: cpp.useCxxPrecompiledHeader
        prefix: precompPath + '/'
        files: ['precomp.h']
        fileTags: ['cpp_pch_src']
    }

    Rule {
        condition: product.defaultCopyHeader
        inputs: [ 'hpp.in' ]
        Artifact {
            filePath: product.includePath  + '/' + FileInfo.relativePath(
                          FileInfo.joinPaths(product.sourceDirectory, product.relativeDirectory),
                          input.filePath)
            fileTags: [ 'hpp' ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = 'coping ' + input.fileName;
            //cmd.silent = true;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }

    Export {
        condition: product.defaultExport
        Depends { name: 'cpp' }
        cpp.defines: [ product.name.toUpperCase() + '_LIB' ]
        cpp.includePaths: [ FileInfo.joinPaths(project.sourceDirectory, 'include', product.module) ]
        cpp.dynamicLibraries: [
            FileInfo.joinPaths(project.sourceDirectory, 'lib', product.module,
                               product.targetName) + product.cpp.variantSuffix
        ]
    }
}
