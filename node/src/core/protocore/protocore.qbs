import qbs
import qbs.File
import qbs.FileInfo

ElectronLibrary {
    readonly property path sourceDir: FileInfo.joinPaths(project.sourceDirectory, 'src/core/icdcore/')

    destinationDir: FileInfo.joinPaths(project.sourceDirectory, '/node/lib/', qbs.buildVariant)
    cpp.includePaths: base.concat([ '.', sourceDir + '/..', sourceDir + '/3rdpart/jsoncpp' ])
    cpp.defines: base.concat([ 'ICDCORE_JSON_LIB' ])

    Group {
        name: 'source'
        prefix: sourceDir
        files: [ '**/*.h', '**/*.cpp' ]
    }

    Group {
        name: 'wrap'
        files: [ '**/*.h', '**/*.cpp' ]
    }
}
