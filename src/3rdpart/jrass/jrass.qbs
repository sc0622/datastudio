import qbs
import qbs.FileInfo
import qbs.Utilities

ThirdDyLibrary {
    version: '1.0.0'
    condition: Utilities.versionCompare(Qt.core.version, '5.5.1') == 0

    Depends { name: '3rdpart.rass' }

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

    cpp.cxxStandardLibrary: 'c++98'
}
