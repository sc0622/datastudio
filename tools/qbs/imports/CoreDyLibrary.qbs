import qbs
import qbs.File
import qbs.FileInfo

CppDyLibrary {
    module: 'core'
    destinationDirectory: FileInfo.joinPaths(project.sourceDirectory, 'lib', module)
}
