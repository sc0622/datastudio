import qbs
import qbs.FileInfo

CoreDyLibrary {

    Depends { name: 'Qt.printsupport' }
    Depends {
        condition: qbs.targetOS.contains('windows')
        name: 'Qt.axcontainer'
    }
    Depends { name: 'icdcore' }
    Depends { name: 'icdworker' }

    Group {
        name: 'Headers'
        files: [ '**/*.h' ]
        excludeFiles: [ '**/private/**/*.h', '**/*_p.h' ]
        fileTags: ['hpp.in' ]
    }

    Group {
        name: 'Headers_private'
        files: [ '**/private/**/*.h', '**/*_p.h' ]
        excludeFiles: [
            '**/excel/**/*.h'
        ]
    }

    Group {
        name: 'Sources'
        files: [ '**/*.cpp' ]
        excludeFiles: [
            '**/excel/**/*.cpp'
        ]
    }

    Group {
        name: 'Resources'
        files: [ 'resource/*.qrc' ]
    }

    Group {
        id: tinyxml
        name: '3rdpart_tinyxml'
        prefix: project.sourceDirectory + '/src/3rdpart/tinyxml/'
        files: [ '*.h', '*.cpp' ]
    }

    cpp.cxxStandardLibrary: 'c++98'
        cpp.includePaths: base.concat([FileInfo.joinPaths('..'), tinyxml.prefix + '/..'])
}
