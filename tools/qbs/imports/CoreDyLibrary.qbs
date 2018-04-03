import qbs
import qbs.File
import qbs.FileInfo

CppDyLibrary {
    module: 'core'
    destinationDirectory: project.sourceDirectory + '/lib/' + module
}
