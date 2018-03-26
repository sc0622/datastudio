import qbs

JCppDyLibrary {
    modulePath: 'core'
    targetName: 'icdcomm'

    Depends { name: 'Qt.gui' }
    Depends { name: 'Qt.network' }

    Group {
        name: 'headers'
        files: [ '*.h' ]
        excludeFiles: [ 'private/*.h', '*_p.h' ]
        fileTags: ['hpp.in' ]
    }

    Group {
        name: 'private'
        files: [ 'private/*.h', '*_p.h' ]
    }

    Group {
        name: 'sources'
        files: [ '*.cpp', 'private/*.cpp' ]
    }

    cpp.cxxStandardLibrary: 'c++98'
    cpp.includePaths: base.concat([])
    cpp.libraryPaths: base.concat([ sourceDirectory + '/private/moxa' ])
    cpp.dynamicLibraries: base.concat([ 'pcomm', 'icdcore' + project.dynamicSuffix ])
    cpp.defines: base.concat([ 'ICDCORE_LIB', 'SERIAL_USE_PCOMM' ])

    Group {
        name: "Special Group"
        qbs.installDir: {
            console.warn(outer);
            return '';
        }
    }
}
