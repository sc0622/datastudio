import qbs
import qbs.File
import qbs.FileInfo

CppDyLibrary {
    module: '3rdpart'
    destinationDirectory: project.sourceDirectory + '/lib/' + module
}
