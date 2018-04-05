import qbs
import qbs.FileInfo

CoreDyLibrary {
    version: '1.0.0'
    defaultExport: false

    Group {
        name: 'Headers'
        files: [ '**/*.h', '3rdpart/jsoncpp/**/*.h']
        excludeFiles: [ '**/private/**/*.h', '**/*_p.h' ]
        fileTags: ['hpp.in' ]
    }

    Group {
        name: 'Headers_private'
        files: [ '**/private/**/*.h', '**/*_p.h' ]
    }

    Group {
        name: 'Sources'
        files: [ '**/*.cpp' ]
    }

    cpp.cxxStandardLibrary: 'c++98'
    cpp.includePaths: base.concat(['3rdpart/jsoncpp'])
    cpp.defines: base.concat([ 'JSON_DLL_BUILD' ])

    Export {
        Depends { name: 'cpp' }
        cpp.defines: {
            var upperName = product.name.toUpperCase();
            var defines = [];
            defines.push(upperName + '_LIB');
            defines.push(upperName + '_JSON_LIB');
            return defines;
        }
        cpp.includePaths: [ FileInfo.joinPaths(project.sourceDirectory,
                                               'include', product.module) ]
        cpp.dynamicLibraries: [
            FileInfo.joinPaths(project.sourceDirectory, 'lib', product.module,
                               product.targetName) + product.cpp.variantSuffix
        ]
    }
}
