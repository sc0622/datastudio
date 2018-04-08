import qbs

Module {
    Depends { name: 'cpp' }
    Depends { name: 'Qt.core' ; cpp.link: false }
}
