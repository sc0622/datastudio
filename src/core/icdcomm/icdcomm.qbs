import qbs

JCppDyLibrary {
    modulePath: 'core'
    moduleName: 'icdcomm'

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
    cpp.includePaths: [
        project.sourceDirectory + '/include/core'
    ]
    cpp.libraryPaths: [
        sourceDirectory + '/private/moxa'
    ]
    cpp.dynamicLibraries: [
        'pcomm',
        'icdcore' + project.dynamicSuffix
    ]
    cpp.defines: [
        'ICDCOMM_LIB',
        'ICDCOMM_BUILD',
        'SERIAL_USE_PCOMM'
    ]
}
