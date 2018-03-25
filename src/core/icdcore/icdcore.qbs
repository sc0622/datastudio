import qbs

JCppDyLibrary {
    modulePath: 'core'
    moduleName: 'icdcore'

    Group {
        name: 'headers'
        files: [
            '*.h',
            'private/*.h',
            '3rdpart/jsoncpp/**/*.h'
        ]
        excludeFiles: [ 'private/*.h', '*_p.h' ]
        fileTags: ['hpp.in' ]
    }

    Group {
        name: 'private'
        files: [
            'private/*.h',
            '*_p.h'
        ]
    }

    Group {
        name: 'sources'
        files: [
            '*.cpp',
            'private/*.cpp',
            '3rdpart/jsoncpp/**/*.cpp'
        ]
    }

    cpp.cxxStandardLibrary: 'c++98'
    cpp.includePaths: [
        '3rdpart/jsoncpp'
    ]
    cpp.defines: [
        'ICDCORE_LIB',
        'ICDCORE_BUILD'
    ]
}
