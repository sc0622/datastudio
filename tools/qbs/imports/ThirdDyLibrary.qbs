import qbs
import qbs.File
import qbs.FileInfo

CppDyLibrary {
    module: '3rdpart'
    destinationDirectory: FileInfo.joinPaths(project.sourceDirectory, 'lib', module)
}
