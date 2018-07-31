import qbs

CoreDyLibrary {

    Depends { name: 'Qt.core' }
    Depends { name: 'Qt.gui' }
    Depends { name: 'Qt.network' }

    Group {
        name: 'Headers'
        files: [ '**/*.h' ]
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

    //cpp.cxxStandardLibrary: 'c++98'
    cpp.libraryPaths: base.concat([ sourceDirectory + '/private/moxa' ])
    cpp.dynamicLibraries: base.concat([ 'pcomm' + (qbs.architecture == 'x86_64' ? '_x86_64' : '') ])
    cpp.defines: base.concat([ 'SERIAL_USE_PCOMM' ])
}
