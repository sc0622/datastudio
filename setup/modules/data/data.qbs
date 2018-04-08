import qbs
import qbs.FileInfo

Module {

    Depends { name: 'cpp' }
    Depends { name: 'Qt.core' ; cpp.link: false }
    Depends { name: 'meta'; cpp.link: false}
 }
