import qbs
import qbs.FileInfo

CoreDyLibrary {
    useQt: false

    translations: [ 'zh_CN.ts' ]
    defaultTranslation: useQt

    Depends {
        condition: useQt && qbs.targetOS.contains('windows')
        name: 'Qt.axcontainer'
    }
    Depends { name: 'icdcore' }

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
        condition: useQt
        name: 'Resources'
        files: [ 'resource/*.qrc' ]
    }

    Group {
        id: tinyxml
        name: '3rdpart_tinyxml'
        prefix: project.sourceDirectory + '/src/3rdpart/tinyxml/'
        files: [ '*.h', '*.cpp' ]
    }

    Properties {
        condition: qbs.targetOS.contains('windows')
        cpp.cxxStandardLibrary: 'c++98'
    }
    cpp.includePaths: base.concat([FileInfo.joinPaths('..'), tinyxml.prefix + '/..'])
}
